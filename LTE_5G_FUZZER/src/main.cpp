/* Standard headers */
#include <filesystem>
#include <iostream>

/* Linux headers */
#include <unistd.h>

/* Project headers */
#include "helpers.h"
#include "my_logger.h"

#include "wdissector.h"
 #include "shm/fuzz_config.h"

#include "Communication/shm_layer_communication.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Settings/serial_config.h"
#include "Configs/Fuzzing_Settings/srsran_config.h"
#include "Configs/Fuzzing_Settings/timers_config.h"
#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"

#include "Coordinators/base_coordinator.h"
#include "Coordinators/coordinator_factory.h"

extern Main_Config main_config_g;
extern SRSRAN_Config srsran_config_g;
extern Serial_Config serial_config_g;
extern Timers_Config timers_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;

extern My_Logger my_logger_g;
static bool OVER = false;
void intHandler(int dummy) {
    std::cout << "\nWe received your signal. The program will terminate soon! Please be patient!" << std::endl;
    SHM_Layer_Communication::is_active = 0;
    OVER = true;
}


int main(int argc, char* argv[]) {

    // Check if we are root
    // if (getuid()) return -10;

    /* Read fuzzing settings configs */
    if (!helpers::parse_json_file_into_class_instance("configs/Fuzzing_Settings/main_config.json", main_config_g)) return -1;
    if (!helpers::parse_json_file_into_class_instance(main_config_g.srsran_config_path, srsran_config_g)) return -1;
    if (!helpers::parse_json_file_into_class_instance(main_config_g.timers_config_path, timers_config_g)) return -1;
    if (!helpers::parse_json_file_into_class_instance(main_config_g.serial_config_path, serial_config_g)) return -1;

    /* Read config regarding interception layers */
    if (!parse_fuzz_config()) return -1;

    /* Init logging. The base folder for logging would be log/TARGET_NAME */
    const std::filesystem::path log_dir_path = std::filesystem::path("log") / std::filesystem::path(main_config_g.target_name);
    if (!my_logger_g.init(log_dir_path)) {
        std::cerr << "Failed to init the logger" << std::endl;
        return -1;
    }

    /* Init the wdissector. For successful initialization, the wdissector library demands having bin/ws directory. */
    if (!helpers::create_directories_if_not_exist("bin/ws")) {
        std::cerr << "Failed to create directory bin/ws" << std::endl;
        return -1;
    }
    if (!wdissector_init(NULL)) {
        std::cerr << "Wdissector init FAILED!" << std::endl;
        return -1;
    }
    wdissector_enable_fast_full_dissection(1);

    /* Save fuzzing strategies' config file names */
    std::vector<std::string> fuzz_strategy_config_filenames;
    if (argc > 1) {
        fuzz_strategy_config_filenames.reserve(argc);
        for (int i = 1; i < argc; i++) {
            fuzz_strategy_config_filenames.push_back(std::string(argv[i]));
        }
    } else {
        std::cerr << "Please specify at least 1 fuzzing strategy file as a command-line argument!" << std::endl;
        return -1;
    }


    //Ensure safe termination of the program
    signal(SIGINT, intHandler);
    std::unique_ptr<Base_Coordinator> coordinator = Coordinator_Factory::get_coordinator_by_protocol_name(main_config_g.protocol_stack_name);
    try {
        if (!coordinator->init(fuzz_strategy_config_filenames)) {
            my_logger_g.logger->error("Failed to init coordinator");
            throw;
        }
        coordinator->fuzzing_loop();
    } catch(const std::exception& ex) {
        my_logger_g.logger->warn("Caught an exception: {}", ex.what());
    }
    coordinator->deinit();

    std::cout << "Exiting... Thanks for using our fuzzer!" << std::endl;
    return 0;
    
}
