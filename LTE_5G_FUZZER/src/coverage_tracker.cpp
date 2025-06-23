#include "coverage_tracker.h"

#include "helpers.h"
#include "my_logger.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Configs/Fuzzing_Settings/main_config.h"

#include <cinttypes>

extern My_Logger my_logger_g;
extern Main_Config main_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;

Coverage_Tracker::Coverage_Tracker(const std::string& name, const std::string& server_address) : name_(name) {
    if (!cov_requester_.init(server_address)) {
        std::cerr << "FAILED TO INIT COVERAGE REQUESTER FOR ADDRESS: " << server_address << std::endl;
        my_logger_g.logger->error("FAILED TO INIT COVERAGE REQUESTER FOR ADDRESS: {}", server_address);
        throw std::runtime_error("FAILED TO INIT COVERAGE REQUESTER");
    }
}

bool Coverage_Tracker::update_coverage_map()
{
    assert(coverage_map_.size() == current_coverage_map_.size());
    bool are_new_buckets_covered = false;
    bool are_new_edges_covered = false;
    
    for (size_t i = 0; i < coverage_map_.size(); i++) {
        // Determine the bucket
        uint32_t hits = current_coverage_map_.at(i);
        if (hits == 0) continue;
        int bucket_num = get_bucket_by_hitcount(hits);

        if (coverage_map_.at(i) == 0 && current_coverage_map_.at(i) != 0) {
            are_new_edges_covered = true;
        }

        if ((coverage_map_.at(i) & (1 << bucket_num)) == 0) {
            // There is no bit in the corresponding bucket
            coverage_map_[i] |= (1 << bucket_num);
            are_new_buckets_covered = true;
        }

        if (are_new_edges_covered && !are_new_buckets_covered) {
            /* Should never happen */
            my_logger_g.logger->error("Coverage error");
            throw std::runtime_error("COVERAGE ERROR");
        }

    }
    if (are_new_edges_covered) {
        if (main_config_g.verbose) printf(GREEN "New edge coverage found!\n" CRESET);
        my_logger_g.logger->debug("New edge coverage found!");
    }
    else {
        //iteration_result.was_new_coverage_found = false;
        if (main_config_g.verbose) printf(RED "New edge coverage NOT found!\n" CRESET);
        my_logger_g.logger->debug("New edge coverage NOT found!");
    }

    uint64_t num_of_filled_buckets = get_number_of_filled_buckets();
    uint64_t num_of_covered_edges = get_current_total_coverage();
    if (main_config_g.verbose) printf(BLUE "Total buckets covered: %" PRIu64 "\n" CRESET, num_of_filled_buckets);
    my_logger_g.logger->debug("Total buckets covered {}", num_of_filled_buckets);

    if (main_config_g.verbose) printf(BLUE "Total edges covered: %" PRIu64 "\n" CRESET, num_of_covered_edges);
    my_logger_g.logger->debug("Total egdes covered {}", num_of_covered_edges);

    //If our feedback is based on buckets (default behavior)
    if (fuzz_strategy_config_g.use_buckets) return are_new_buckets_covered;

    //If our feedback is based on edges
    return are_new_edges_covered;
}

bool Coverage_Tracker::reset_coverage_map()
{
    return cov_requester_.reset_code_coverage_map();
}

uint64_t Coverage_Tracker::get_current_total_coverage()
{
    uint64_t edges_covered = 0;
    for (size_t i = 0; i < coverage_map_.size(); i++) {
        if (coverage_map_.at(i)) edges_covered++;
    }
    return edges_covered;
}

bool Coverage_Tracker::get_edge_amount()
{
    uint32_t total_edges = cov_requester_.get_code_coverage_map_size();
    if (total_edges == 0) {
        my_logger_g.logger->warn("Failed to get the edge amount!");
        return false;
    }
    if (main_config_g.verbose) printf("Total found edges: %u\n", total_edges);
    my_logger_g.logger->debug("Total found edges: {}", total_edges);
    current_coverage_map_.resize(total_edges);
    coverage_map_.resize(total_edges, 0);
    return true;
}

uint64_t Coverage_Tracker::get_number_of_filled_buckets()
{
    uint64_t res = 0;
    for (size_t i = 0; i < coverage_map_.size(); i++) {
        uint8_t cur_value = coverage_map_.at(i);
        while (cur_value != 0) {
            if (cur_value & 1) res++;
            cur_value >>= 1;
        }
    }
    return res;
}

uint32_t Coverage_Tracker::get_edge_coverage()
{
    if (coverage_map_.size() == 0) {
        if (!get_edge_amount()) {
            throw std::runtime_error("Failed to get edge amount");
        }
    }

    if (!cov_requester_.get_code_coverage_map(current_coverage_map_.size(), &current_coverage_map_[0])) {
        printf("Requesting bitmap failed!\n");
        throw std::runtime_error("Requesting bitmap failed!");
    }

    uint32_t total_edges_covered = 0;
    for (size_t i = 0; i < current_coverage_map_.size(); i++) {
        if (current_coverage_map_[i]) total_edges_covered++;
    }
    if (main_config_g.verbose) printf("Edges covered: %u\n", total_edges_covered);
    my_logger_g.logger->debug("Edges covered: {}", total_edges_covered);

    return total_edges_covered;
}

const std::string Coverage_Tracker::get_name() const {
    return name_;
}

int Coverage_Tracker::get_bucket_by_hitcount(size_t hitcount) {
    for (size_t i = bucket_min_hits_.size() - 1; i >= 0; i--) {
        if (hitcount >= bucket_min_hits_.at(i)) return i;
    }
    return -1;
}