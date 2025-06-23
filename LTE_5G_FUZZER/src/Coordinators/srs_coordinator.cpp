#include "Coordinators/srs_coordinator.h"

#include "my_logger.h"
#include "statistics.h"
#include "coverage_tracker.h"
#include "Communication/shm_layer_communication.h"
#include "Communication/srsue_communication.h"
#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Fuzzers/fuzzer_factory.h"

#include "shm/fuzz_config.h"

#include <algorithm>
#include <thread>

extern Fuzz_Config fuzz_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern Main_Config main_config_g;
extern My_Logger my_logger_g;
extern std::string ue_log_file_name_g;
extern Statistics statistics_g;

SRS_Coordinator::SRS_Coordinator() {
    //if (fuzz_strategy_config_g.use_coverage_logging) coverage_trackers.emplace_back(std::make_unique<Coverage_Tracker>("UE_COVERAGE_TRACKER" , "tcp://127.0.0.1:5566"));
    if (fuzz_strategy_config_g.use_coverage_logging) coverage_trackers.emplace_back(std::make_unique<Coverage_Tracker>("ENB_COVERAGE_TRACKER", "tcp://127.0.0.1:5567"));
    if (fuzz_strategy_config_g.use_coverage_logging) coverage_trackers.emplace_back(std::make_unique<Coverage_Tracker>("UE_COVERAGE_TRACKER" , "tcp://127.0.0.1:5566"));
    if (fuzz_strategy_config_g.use_coverage_logging) coverage_trackers.emplace_back(std::make_unique<Coverage_Tracker>("EPC_COVERAGE_TRACKER", "tcp://127.0.0.1:5568"));
}


bool SRS_Coordinator::init(const std::vector<std::string>& fuzz_strategy_config_names)
{
    if (!init_fuzzing_strategies(fuzz_strategy_config_names)) {
        my_logger_g.logger->error("Failed to init the fuzzing strategies");
        return false;
    }

    if (!srsue_comm.init_zmq_communication()) {
        throw std::runtime_error("Error to init ZMQ communication with reference UE!");
    }

    return true;
}

void SRS_Coordinator::thread_dut_communication_func()
{
    //my_logger_g.logger->info("Starting fuzzing with the following configurations:\n{}", config_g); //if info is set sanitizer crashes for some reason
    
    for (const Fuzz_Strategy_Config& fuzz_strategy_config : fuzz_strategy_configs_) {

        if (!prepare_new_fuzzing_sprint(fuzz_strategy_config)) {
            my_logger_g.logger->error("Failed to prepare new fuzzing sprint");
            break;
        }

        consecutive_successful_attach_counter = 0;
        consecutive_hangs_counter = 0;
        consecutive_failed_attach_counter = 0;
        while(SHM_Layer_Communication::is_active) {
            my_logger_g.logger->flush();
            my_logger_g.logger->info("================ START OF A NEW FUZZING ITERATION {} ================", global_iteration);
            
            std::string msg = srsue_comm.receive_msg();
            my_logger_g.logger->info("GOT MESSAGE FROM UE: {}", msg);

            if (msg == "ATTACH FAILED") {
                iteration_result.attach_result = AttachResult::FAIL;
                statistics_g.attach_failed_counter++;
                consecutive_failed_attach_counter++;
                consecutive_successful_attach_counter = 0;
                consecutive_hangs_counter = 0;
            } else if (msg == "ATTACH SUCCESSFUL") {
                iteration_result.attach_result = AttachResult::SUCCESS;
                statistics_g.attach_successful_counter++;
                consecutive_successful_attach_counter++;
                consecutive_hangs_counter = 0;
                consecutive_failed_attach_counter = 0;
            } else if (msg == "") {
                iteration_result.attach_result = AttachResult::UNKNOWN;
                statistics_g.hang_counter++;
                my_logger_g.logger->warn("UNKNOWN HANG HAPPENED!");
                // LOG THE SEED THAT CAUSED HANG (MOST PROBABLY IT WAS PREVIOUS SEED)
                helpers::write_class_instance_to_json_file(Base_Fuzzer::previous_seed, hang_seeds_dir + "hang_seed_" + helpers::get_current_time_stamp() + ".json");
                consecutive_failed_attach_counter = 0;
                consecutive_successful_attach_counter = 0;
                consecutive_hangs_counter++;
            } 
            // else {
            //     iteration_result.attach_result = AttachResult::UNKNOWN;
            //     my_logger_g.logger->error("srsUE Communication failed");
            //     break;
            // }
            
            if (!renew_fuzzing_iteration()) {
                break;
            }
        }
        if (!SHM_Layer_Communication::is_active) break;
    }
    terminate_fuzzing();
}

bool SRS_Coordinator::reset_target() {
    bool ret = false;
    try {
        srsue_comm.reset_stack();
        srsue_comm.disconnect_from_network();
        srsue_comm.connect_to_network();
        ret = true;
    } catch(std::exception& ex) {
        my_logger_g.logger->debug("Caught exception: {}", ex.what());
    }
    wdissector_mutex.unlock(); // TOOO: Get rid of that. Looks awful.
    return ret;
}

void SRS_Coordinator::deinit() {
    return;
}

std::string SRS_Coordinator::get_name() {
    return name_;
}