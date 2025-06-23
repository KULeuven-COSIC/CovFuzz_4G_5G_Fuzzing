#include "Coordinators/active_coordinator.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Communication/shm_layer_communication.h"

#include "coverage_tracker.h"
#include "my_logger.h"
#include "statistics.h"

#include <algorithm>
#include <string>
#include <vector>

extern Main_Config main_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern My_Logger my_logger_g;
extern std::string dut_log_file_name_g;
extern Statistics statistics_g;

Active_Coordinator::Active_Coordinator()
{
        if (fuzz_strategy_config_g.use_coverage_logging || fuzz_strategy_config_g.use_coverage_feedback) coverage_trackers.emplace_back(std::make_unique<Coverage_Tracker>("ENB_COVERAGE_TRACKER", "tcp://127.0.0.1:5567"));
        if (fuzz_strategy_config_g.use_coverage_logging || fuzz_strategy_config_g.use_coverage_feedback) coverage_trackers.emplace_back(std::make_unique<Coverage_Tracker>("EPC_COVERAGE_TRACKER", "tcp://127.0.0.1:5568"));
}

Active_Coordinator::~Active_Coordinator() {}

bool Active_Coordinator::init(const std::vector<std::string>& fuzz_strategy_config_names) {
    if (!init_fuzzing_strategies(fuzz_strategy_config_names)) {
        my_logger_g.logger->error("Failed to init the fuzzing strategies");
        return false;
    }
    return true;
}

void Active_Coordinator::deinit() {
    return;
}

void Active_Coordinator::thread_dut_communication_func() {
    
    consecutive_successful_attach_counter = 0;
    consecutive_failed_attach_counter = 0;
    consecutive_empty_iterations = 0;

    for (const Fuzz_Strategy_Config& fuzz_strategy_config : fuzz_strategy_configs_) {

        if (!prepare_new_fuzzing_sprint(fuzz_strategy_config)) {
            my_logger_g.logger->error("Failed to prepare new fuzzing sprint");
            break;
        }

        while(SHM_Layer_Communication::is_active) {
            
            my_logger_g.logger->info("================ START OF A NEW FUZZING ITERATION {} ================", global_iteration);
            my_logger_g.logger->flush();

            try {

                if (!ue_->connect_to_network()) {
                    if (!ue_->is_alive()) throw std::runtime_error(MODEM_DEAD);
                }

                bool is_connected_to_custom_network = ue_->is_connected();
                
                if (!is_connected_to_custom_network) {
                    iteration_result.attach_result = AttachResult::FAIL;
                    my_logger_g.logger->info("ATTACH FAILED");
                    statistics_g.attach_failed_counter++;
                    consecutive_failed_attach_counter++;
                    if (Base_Fuzzer::current_seed->is_empty()) consecutive_empty_iterations++; // TODO: Change that. Now if fuzzing is NONE it will be considered empty iteration 
                    else consecutive_empty_iterations = 0;
                    consecutive_successful_attach_counter = 0;
                    if (fuzz_state == Fuzzing_State::CHECK_CONNECTIVITY) {
                        my_logger_g.logger->debug("Failed to attach {} times", consecutive_failed_attach_counter);
                        if (consecutive_failed_attach_counter > 5) {
                            my_logger_g.logger->debug("Moving to a HANG_RECOVERY state.");
                            fuzz_state = Fuzzing_State::UE_HANG_RECOVERY;
                        }
                    }
                } else {
                    iteration_result.attach_result = AttachResult::SUCCESS;
                    my_logger_g.logger->info("ATTACH SUCCESSFUL");
                    statistics_g.attach_successful_counter++;
                    consecutive_successful_attach_counter++;
                    consecutive_failed_attach_counter = 0;
                    consecutive_empty_iterations = 0;
                    if (fuzz_state == Fuzzing_State::CHECK_CONNECTIVITY) {
                        my_logger_g.logger->debug("Moving to the NORMAL fuzzing state");
                        fuzz_state = Fuzzing_State::NORMAL;
                    }
                }

                if (consecutive_empty_iterations > 5 && fuzz_state == Fuzzing_State::NORMAL) {
                    my_logger_g.logger->debug("Detected {} consecutive empty iterations. Moving to the CHECK_CONNECTIVITY fuzzing state", consecutive_empty_iterations);
                    fuzz_state = Fuzzing_State::CHECK_CONNECTIVITY;
                }

                if (!renew_fuzzing_iteration()) {
                    break;
                }
            } catch(std::runtime_error& ex) {
                my_logger_g.logger->warn("Exception happened: {}", ex.what());
                if (std::string(ex.what()) == std::string(MODEM_DEAD)) {
                    statistics_g.ue_crash_counter++;
                    if (ue_->restart()) {
                        my_logger_g.logger->info("Restarted the DUT successfully");
                    } 
                    else {
                        terminate_fuzzing();
                        return;
                    }
                } else {
                    terminate_fuzzing();
                    return;
                }
            }
        }
        if (!SHM_Layer_Communication::is_active) break;
    }
    terminate_fuzzing();
}

bool Active_Coordinator::reset_target()
{
    wdissector_mutex.unlock();
    for (int cur_try = 0; ; cur_try++) {
        //if (dut_comm->disconnect_from_network()) break;
        if (ue_->disconnect_from_network()) break;
        if (cur_try == max_tries) break;//throw std::runtime_error("DUT COULDN'T DISCONNECT FROM THE NETWORK");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return true;
}

std::string Active_Coordinator::get_name() {
    return name_;
}