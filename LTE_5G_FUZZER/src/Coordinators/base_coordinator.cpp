#include "Coordinators/base_coordinator.h"

#include "statistics.h"
#include "my_logger.h"
#include "field.h"
#include "Communication/shm_layer_communication_factory.h"
#include "Communication/shm_layer_communication.h"
#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Configs/Fuzzing_Settings/main_config.h"

#include "Protocol_Stack/protocol_stack_factory.h"
#include "UE/UE_factory.h"

#include "Fuzzers/base_fuzzer.h"

#include "shm/fuzz_config.h"

#include <nlohmann/json.hpp>

#include <stdlib.h>

extern My_Logger my_logger_g;
extern Main_Config main_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern Fuzz_Config fuzz_config_g;
extern Statistics statistics_g;

void to_json(nlohmann::json &j, const std::unordered_map<std::string, std::vector<Field> >& database) {
    for (auto& it : database) {
        j[it.first] = it.second;
    }
}
void from_json(const nlohmann::json &j, std::unordered_map<std::string, std::vector<Field> >& database) {
    for (auto& it : j.items()) {
        database[it.key()] = it.value().get<std::vector<Field> >();
    }
}

Base_Coordinator::Base_Coordinator()
{
    if (!helpers::create_directories_if_not_exist(hang_seeds_dir)) {
        my_logger_g.logger->error("Failed to create a directory: {}", hang_seeds_dir);
        throw std::runtime_error("Failed to create a directory!");
    }
    if (!helpers::create_directories_if_not_exist(crash_seeds_dir)) {
        my_logger_g.logger->error("Failed to create a directory: {}", crash_seeds_dir);
        throw std::runtime_error("Failed to create a directory!");
    }
    protocol_stack = Protocol_Stack_Factory::get_protocol_stack_by_protocol_name(main_config_g.protocol_stack_name);
    if (!protocol_stack->start()) throw std::runtime_error("Could not start the protocol stack!");
    ue_ = UE_Factory::get_ue_by_protocol_name(main_config_g.ue_name);
    if (!ue_->start()) throw std::runtime_error("Could not start the UE!");
}

Base_Coordinator::~Base_Coordinator()
{
    protocol_stack->stop();
    ue_->stop();
}

bool Base_Coordinator::init_fuzzing_strategies(const std::vector<std::string> &fuzz_strategy_config_names)
{
    fuzz_strategy_configs_.reserve(fuzz_strategy_config_names.size());
    for (int i = 0, i_max = fuzz_strategy_config_names.size(); i < i_max; ++i) {
        const std::string& fuzz_strategy_config_name = fuzz_strategy_config_names.at(i);
        Fuzz_Strategy_Config config;
        if (!helpers::parse_json_file_into_class_instance(fuzz_strategy_config_name, config)) {
            my_logger_g.logger->error("Failed to read config file: {}", fuzz_strategy_config_name);
            return false;
        }
        fuzz_strategy_configs_.push_back(std::move(config));
    }
    return true;
}

bool Base_Coordinator::renew_fuzzing_iteration()
{
    bool need_to_finish = false;
    bool need_to_restart_protocol_stack = false;
    bool need_to_restart_ue = false;
    iteration_result.was_crash_detected = false;

    /* 1. Check aliveliness (DUT and Protocol Stack) */
    if (!protocol_stack->is_running()) {
        std::string crash_seed_filename = crash_seeds_dir + "stack_crash_seed_";
        helpers::write_class_instance_to_json_file(Base_Fuzzer::current_seed, crash_seed_filename + helpers::get_current_time_stamp() + ".json");
        iteration_result.was_crash_detected = true;
        need_to_restart_protocol_stack = true;
    }

    if (!ue_->is_alive()) {
        std::string hang_seed_filename = hang_seeds_dir + "stack_hang_seed_";
        helpers::write_class_instance_to_json_file(Base_Fuzzer::current_seed, hang_seed_filename + helpers::get_current_time_stamp() + ".json");
        iteration_result.was_crash_detected = true;
        need_to_restart_ue = true;
        statistics_g.ue_crash_counter++;
    }

    my_logger_g.logger->info("Completed fuzzing iteration: {} ({} for current config file)", global_iteration, local_iteration);

    /* 2. Update fuzzing "State" */
    global_iteration++;
    local_iteration++;
    wdissector_mutex.lock();

    /* Update coverage information and tweak probabilities */
    try {
        if (fuzz_strategy_config_g.use_coverage_logging) update_coverage_information();
        if (fuzz_strategy_config_g.use_probability_optimization) {
            update_probabilities(iteration_result.was_new_coverage_found);
        }
    } catch(...) {
        need_to_restart_protocol_stack = true;
    }

    Base_Fuzzer::mut_field_num_tracker.push_mutated_field_num(Base_Fuzzer::mutated_fields_num);

    my_logger_g.logger->debug("Number of mutated fields in this iteration: {}", Base_Fuzzer::mutated_fields_num);
    Base_Fuzzer::mutated_fields.clear();
    Base_Fuzzer::mutated_fields_num = 0;

    if (fuzz_strategy_config_g.use_probability_resets && Base_Fuzzer::mut_field_num_tracker.needs_reset()) {
        /* Reset the probabilities. This is done by deleting all the packets in the database.*/
        Base_Fuzzer::mut_field_num_tracker.reset();
        my_logger_g.logger->info("Resetting the probabilities");
        Base_Fuzzer::packet_fields_database.clear();
    }

    /* 3. Check if any of the fuzzers requests finish of the fuzzing */
    for (size_t i = 0; i < fuzzers.size(); i++) {
        bool need_to_finish_local = !fuzzers[i]->prepare_new_iteration();
        need_to_finish |= need_to_finish_local;
        if (need_to_finish_local) my_logger_g.logger->info("Fuzzer indexed {} requested finishing fuzzing", i);
    }

    //wdissector_mutex.unlock();

    if (!need_to_restart_protocol_stack) {
        try {
            if (!reset_target()) need_to_restart_protocol_stack = true; // TODO: Why is it needed?
        } catch(...) {
            throw;
        }
    } else wdissector_mutex.unlock();

    if (!main_config_g.verbose) {
        if (statistics_g.has_this_iteration_failed) statistics_g.failed_fuzz_iterations_counter++;
        helpers::clear_screen(); print_statistics();
        statistics_g.has_this_iteration_failed = false;
    }

    need_to_finish |= (static_cast<int>(local_iteration) >= (fuzz_strategy_config_g.attach_attempts_per_iteration == -1 ? INT_MAX : fuzz_strategy_config_g.attach_attempts_per_iteration));
    if (need_to_finish) {
        my_logger_g.logger->info("Finishing fuzzing");
        my_logger_g.logger->debug("Current state machine:");
        my_logger_g.logger->flush();
    }

    my_logger_g.logger->flush();

    //wdissector_mutex.unlock();
    if (consecutive_successful_attach_counter == fuzz_strategy_config_g.restart_after_consecutive_successful_attaches ||
        consecutive_failed_attach_counter == fuzz_strategy_config_g.restart_after_consecutive_failed_attaches ||
        consecutive_hangs_counter == fuzz_strategy_config_g.restart_after_consecutive_hangs ) { // Smth suspicious is going on; RESET everything
        need_to_restart_protocol_stack = true;
    }

    if (need_to_restart_protocol_stack)  {
        if (!protocol_stack->restart()) {
            my_logger_g.logger->error("Failed to restart the protocol stack!");
            return false;
        }
    }
    if (need_to_restart_ue) {
        if (!ue_->restart()) {
            my_logger_g.logger->error("Failed to restart the UE!");
            return false;
        }
    }

    return !need_to_finish;
}

void Base_Coordinator::layer_fuzzing_loop(EnumMutex mutex_num, bool FUZZ) {

    Packet::protocol_type = get_protocol_type_by_protocol_stack_name(main_config_g.protocol_stack_name);
    const std::string layer_name = helpers::get_layer_name_by_idx(mutex_num);
    my_logger_g.logger->info("Starting {} thread", layer_name);
    std::unique_ptr<SHM_Layer_Communication> SHM_Comm = SHM_Layer_Communication_Factory::get_shm_layer_communication_instance_by_layer_num(mutex_num);
    my_logger_g.logger->info("Inited communication");
    
    std::string dissector;
    if (layer_name == "MAC") dissector = helpers::get_MAC_dissector(Packet::protocol_type);

    while (SHM_Layer_Communication::is_active) {
        bool failed = false;

        /* Recieve intercepted message */
        Packet pdu = SHM_Comm->receive();

        if (!pdu.get_size()) continue;

        if (layer_name == "RRC") {
            dissector = helpers::get_RRC_dissector_by_channel(pdu.get_channel(), Packet::protocol_type);
            if (dissector.empty()) {
                my_logger_g.logger->error("Could not find a dissector for RRC channel {}", pdu.get_channel());
            }
        }
        if (dissector.empty()) {
            my_logger_g.logger->error("Could not find a dissector for layer {}", layer_name);
            return;
        }

        pdu.set_dissector_name(dissector);

        wdissector_mutex.lock();
        my_logger_g.logger->info("[{}] Dissector {} {} (Direction: {})", layer_name, dissector, pdu, helpers::get_direction_by_idx(pdu.get_direction()));

        if (FUZZ && helpers::is_fuzzing_direction(pdu.get_direction())) {
            if (!pdu.full_dissect()) {
                my_logger_g.logger->warn("[{}] Fuzz iteration failed in prepare_fuzz", layer_name);
                failed = true;
            }
            my_logger_g.logger->info("Dissector's summary: {}", pdu.get_summary());
            for (size_t i = 0; i < fuzzers.size(); i++) {
                if (!fuzzers.at(i)->fuzz(pdu)) {
                    my_logger_g.logger->warn("[{}] Fuzz iteration failed", layer_name);
                    failed = true;
                }
            }
            std::string fuzzed_packet_type = "UNKNOWN";
            if (pdu.quick_dissect()) {
                fuzzed_packet_type = pdu.get_summary_short();
            }
            if (helpers::is_direction_logged(pdu.get_direction())) {
                my_logger_g.logger->info("Fuzzed packet: {} (type {})", pdu, fuzzed_packet_type);
            }

        } else {
            if (pdu.quick_dissect()) {
                my_logger_g.logger->info("Dissector's summary: {}", pdu.get_summary());
            } else {
                my_logger_g.logger->warn("Failed to dissect the packet! (dissector: {})", dissector);
            }
        }
        my_logger_g.logger->info("");
        my_logger_g.logger->flush();

        if (FUZZ && helpers::is_fuzzing_direction(pdu.get_direction())) {
            Base_Fuzzer::packet_buffer[pdu.get_dissector_name()].insert(pdu);
        }
        wdissector_mutex.unlock();
        SHM_Comm->send(pdu);
        if (failed) statistics_g.has_this_iteration_failed = true;
    }
}

void Base_Coordinator::terminate_fuzzing()
{
    SHM_Layer_Communication::is_active = 0;
}

void Base_Coordinator::fuzzing_loop()
{

    // Dissector warm-up. For some reason, wdissector does not dissect the first packet correctly.
    auto p = helpers::get_sample_packet();
    if (!p.quick_dissect()) {
        my_logger_g.logger->error("Dissector failed to dissect a sample packet!");
        return;
    }

    std::unique_ptr<std::thread> thread_mac;
    std::unique_ptr<std::thread> thread_rrc;

    bool mac_thread_is_running = false;
    bool rrc_thread_is_running = false;

    // std::unique_ptr<std::thread> thread_dut_communication = std::make_unique<std::thread>(std::thread(&Base_Coordinator::thread_dut_communication_func, this));
    // thread_dut_communication->detach();

    bool fuzz_MAC = fuzz_config_g.FUZZ_MAC_DL || fuzz_config_g.FUZZ_MAC_UL || fuzz_config_g.FUZZ_MIB || fuzz_config_g.FUZZ_SIB;
    bool fuzz_RRC = fuzz_config_g.FUZZ_RRC_DL || fuzz_config_g.FUZZ_RRC_UL;

    if (fuzz_MAC) {
        mac_thread_is_running = true;
        thread_mac = std::make_unique<std::thread>(&Base_Coordinator::layer_fuzzing_loop, this, SHM_MUTEX_MAC, fuzz_MAC);
    }

    if (fuzz_RRC) {
        rrc_thread_is_running = true;
        thread_rrc = std::make_unique<std::thread>(&Base_Coordinator::layer_fuzzing_loop, this, SHM_MUTEX_RRC, fuzz_RRC);
    }

    //try {
    thread_dut_communication_func();
    // } catch (std::exception& ex) {
    //     my_logger_g.logger->error("Exception happened during thread_dut_communication_func: {}", ex.what());
    // }

    if (mac_thread_is_running) {
        thread_mac->join();
    }

    if (rrc_thread_is_running) {
        thread_rrc->join();
    }

}

double Base_Coordinator::coverage_formula_1(double beta) {
    if (Base_Fuzzer::mutated_fields_num == 0) return 0.0;
    double score = 0;

    double normalized_value = std::min(1.0, (double)local_iteration / 2000);
    double cov_formula = beta * normalized_value;

    if (iteration_result.was_new_coverage_found) score += cov_formula / Base_Fuzzer::mutated_fields_num;
    else score -= (1.0 / cov_formula) / Base_Fuzzer::mutated_fields_num;

    return score;
}

double Base_Coordinator::calculate_field_mutation_score() {
    double beta = fuzz_strategy_config_g.beta;
    if (fuzz_strategy_config_g.use_coverage_feedback) {
        if (fuzz_strategy_config_g.beta_2 != 0.0 && (double)local_iteration / fuzz_strategy_config_g.attach_attempts_per_iteration > 0.5) beta = fuzz_strategy_config_g.beta_2;
        return coverage_formula_1(beta);
    }
    return 0.0;
}

void Base_Coordinator::update_probabilities(bool found_new_coverage) {
    if (Base_Fuzzer::mutated_fields.size() == 0) return;
    double score = calculate_field_mutation_score();
    my_logger_g.logger->debug("Update score: {}", score);
    std::vector<Field*>::reverse_iterator it;
    for (it = Base_Fuzzer::mutated_fields.rbegin(); it != Base_Fuzzer::mutated_fields.rend(); it++) {
        if ((*it) != nullptr) {
            double prob_change = score / log2((*it)->max_value + 1);
            my_logger_g.logger->debug("Field {} (idx {}): change={}", (*it)->field_name, (*it)->index, prob_change);
            (*it)->set_mutation_probability((*it)->mutation_probability + prob_change);
        }
    }
}

void Base_Coordinator::update_coverage_information() {
    if (statistics.is_open()) {
        statistics << std::endl << helpers::get_current_time_stamp();
    }
    iteration_result.was_new_coverage_found = false;
    uint8_t idx = 0;
    for (auto& cov_tracker_ptr : coverage_trackers) {
        uint32_t covered_edges_num = cov_tracker_ptr->get_edge_coverage(); // throws an exception if unsuccessful
        if (statistics.is_open()) {
            statistics << "," << covered_edges_num << ",";
        }
        Base_Fuzzer::current_seed->edges_covered = covered_edges_num;
        //bool new_coverage_found = false;
        uint64_t old_filled_buckets = cov_tracker_ptr->get_number_of_filled_buckets();
        if (cov_tracker_ptr->update_coverage_map()) {
            //if (cov_tracker_ptr->get_name() == "ENB_COVERAGE_TRACKER" || cov_tracker_ptr->get_name() == "EPC_COVERAGE_TRACKER") new_coverage_found = true;
            my_logger_g.logger->info("{}: NEW COVERAGE FOUND!", cov_tracker_ptr->get_name());
            if (main_config_g.verbose) printf(GREEN "%s: New coverage found!\n" CRESET, cov_tracker_ptr->get_name().c_str());
        } else {
            my_logger_g.logger->info("{}: NEW COVERAGE NOT FOUND!", cov_tracker_ptr->get_name());
            if (main_config_g.verbose) printf(RED "%s: New coverage NOT found!\n" CRESET, cov_tracker_ptr->get_name().c_str());
        }
        
        uint64_t tec = cov_tracker_ptr->get_current_total_coverage();
        uint64_t new_filled_buckets = cov_tracker_ptr->get_number_of_filled_buckets();
        if (idx == 0) {
            iteration_result.new_coverage_found = new_filled_buckets - old_filled_buckets;
            iteration_result.was_new_coverage_found = (iteration_result.new_coverage_found != 0);
            // new_coverage_found = true;
        }

        if (cov_tracker_ptr->get_name() == "UE_COVERAGE_TRACKER") {
            my_logger_g.logger->info("Total explored edges now: {}", tec);
            my_logger_g.logger->info("Total explored buckets now: {}", new_filled_buckets);
        }
        if (statistics.is_open()) {
            statistics << tec << "," << new_filled_buckets;// << "," << fuzzer->get_number_of_saved_patches();
        }
        //fuzzer->current_seed = std::make_shared<Seed>(); //It is fuzzer's responsibility to do so
        cov_tracker_ptr->reset_coverage_map();
        idx++;
    }
    //return new_coverage_found;
}

// void to_json(json &j, const std::unordered_map<std::string, std::vector<Field>> &database)
// {
//     for (const auto& [name, vec_f] : database) {
//         j[name] = json::array();
//         for (const auto& f : vec_f) {
//             j[name].push_back(json{{"name", f.field_name}, {"index", f.index}, {"max_value", f.max_value}, {"mutation_probability", f.mutation_probability}});
//         }
//     }
// }


bool Base_Coordinator::prepare_new_fuzzing_sprint(const Fuzz_Strategy_Config& fuzz_strategy_config)
{
    fuzz_strategy_config_g = fuzz_strategy_config;
    local_iteration = 0;
    fuzzers.clear();
    for (const FuzzingStrategy fuzz_strategy : fuzz_strategy_config_g.fuzzing_strategies) {
        fuzzers.push_back(Fuzzer_Factory::get_fuzzer_by_fuzzing_strategy(fuzz_strategy));
    }
    //fuzzer.reset(get_fuzzer_instance_by_fuzzing_strategy(fs));
    for (size_t i = 0; i < fuzzers.size(); i++) {
        if (fuzz_strategy_config_g.use_existing_seeds) {
            if (fuzz_strategy_config_g.seed_paths.size() > i) fuzzers.at(i)->set_seed_path(fuzz_strategy_config_g.seed_paths.at(i));
            else my_logger_g.logger->warn("Could not use an existing seed. Please check the seeds_paths variable in the config file.");
        }
        if (!fuzzers.at(i)->init()) {
            printf(RED "Failed to initialize fuzzer\n" CRESET);
            return false;
        }
    }

    if (statistics_file_name.empty()) {
        if (fuzz_strategy_config_g.coverage_log_path != "") {
            if (system(("mkdir -p " + fuzz_strategy_config_g.coverage_log_path).c_str())) {
                throw std::runtime_error("Could not create coverage log directory");
            }
            statistics_file_name = fuzz_strategy_config_g.coverage_log_path + "/coverage_statistics_" + helpers::get_current_time_stamp();
        } else {
            statistics_file_name = "/tmp/coverage_statistics_" + helpers::get_current_time_stamp();
        }

        if (fuzz_strategy_config_g.use_existing_patches) statistics_file_name += "_PREPATCHED";
        statistics.open(statistics_file_name);
    }

    save_all_seeds_to.open(save_all_seeds_to_filename, std::ios::trunc);

    return true;
}

void Base_Coordinator::print_statistics() {
    std::string total_iterations = (fuzz_strategy_config_g.attach_attempts_per_iteration > 0) ? std::to_string(fuzz_strategy_config_g.attach_attempts_per_iteration) : "∞";
    std::cout << std::setw(40) << std::left << "\tIteration: " + std::to_string(local_iteration) + "/" + total_iterations + " (" + std::to_string(global_iteration) + " in total)"; 
    std::cout << std::setw(30) << std::left << "Successful attaches: " + std::to_string(statistics_g.attach_successful_counter)  << std::endl;
    
    if (fuzz_strategy_config_g.use_coverage_logging) std::cout << std::setw(40) << std::left << "\tTotal edges explored: " + std::to_string(coverage_trackers.at(0)->get_current_total_coverage());
    std::cout << std::setw(30) << std::left << "Failed attaches: " + std::to_string(statistics_g.attach_failed_counter)  << std::endl;
    
    if (fuzz_strategy_config_g.use_coverage_logging) std::cout << std::setw(40) << std::left << "\tTotal buckets explored: " + std::to_string(coverage_trackers.at(0)->get_number_of_filled_buckets());
    std::cout << std::setw(30) << std::left << "Hangs: " + std::to_string(statistics_g.hang_counter)  << std::endl;
    
    std::cout << std::setw(40) << std::left << RED "\tCrashes (UE/ENB/EPC): " + std::to_string(statistics_g.ue_crash_counter) + "/"
        + std::to_string(statistics_g.enb_crash_counter) + "/" + std::to_string(statistics_g.epc_crash_counter);
    std::cout << std::setw(30) << std::left << RED "Failed fuzz iterations: " + std::to_string(statistics_g.failed_fuzz_iterations_counter) + std::string(CRESET)  << std::endl;

    //std::cout << std::setw(40) << std::left << YELLOW "\tPatch queue size: " + std::to_string(fuzzer->get_number_of_saved_patches()) + std::string(CRESET)  << std::endl;
    
    std::cout << "\n\n";

    std::cout << std::setw(40) << std::left << BLUE "\tMutation statistics:" CRESET << std::endl;

    std::cout << std::setw(40) << std::left << "\tRandom mutations: " + std::to_string(statistics_g.rand_mutator_counter);
    std::cout << std::setw(30) << std::left << "Fuzzing strategies: " + get_all_fuzzing_strategy_names(fuzz_strategy_config_g.fuzzing_strategies)  << std::endl;

    std::cout << std::setw(40) << std::left << "\tMin mutations: " + std::to_string(statistics_g.min_mutator_counter);
    std::cout << std::endl;

    std::cout << std::setw(40) << std::left << "\tMax mutations: " + std::to_string(statistics_g.max_mutator_counter);
    std::cout << std::endl;

    std::cout << std::setw(40) << std::left << "\tAdd mutations: " + std::to_string(statistics_g.add_mutator_counter);
    std::cout << std::endl;

    std::cout << std::setw(40) << std::left << "\tSub mutations: " + std::to_string(statistics_g.sub_mutator_counter);
    std::cout << std::endl;

    std::cout << std::setw(40) << std::left << "\tInjections: " + std::to_string(statistics_g.injection_counter);
    std::cout << std::endl;
}