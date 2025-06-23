#pragma once

#include "coverage_requester.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>


#define NUM_OF_BUCKETS 8

/* Class responsible for tracking the coverage of one running instance. It uses ZMQ sockets to communicate the coverage information. */
class Coverage_Tracker {
public:
    Coverage_Tracker(const std::string& name, const std::string& server_address);
    
    bool update_coverage_map();
    bool reset_coverage_map();
    uint64_t get_current_total_coverage();
    bool get_edge_amount();
    uint64_t get_number_of_filled_buckets();
    uint32_t get_edge_coverage();

    const std::string get_name() const;

private:
    std::string name_;

    Coverage_Requester cov_requester_;
    std::vector<uint8_t> current_coverage_map_;
    std::vector<uint8_t> coverage_map_;

    int get_bucket_by_hitcount(size_t hitcount);

    const std::array<size_t, NUM_OF_BUCKETS> bucket_min_hits_ = {1, 2, 3, 4, 8, 16, 32, 128};
};