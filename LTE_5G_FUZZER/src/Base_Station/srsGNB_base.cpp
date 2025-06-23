#include "Base_Station/srsGNB_base.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Settings/srsran_config.h"
#include "helpers.h"
#include "my_logger.h"

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <thread>

extern Main_Config main_config_g;
extern My_Logger my_logger_g;
extern SRSRAN_Config srsran_config_g;

bool SRSGNB_Base::stop()
{
    if (!is_alive()) {
        my_logger_g.logger->warn("{} is not alive. Nothing to stop.", get_name());
        return true;
    }

    int signal = SIGKILL;
    std::string command = "sudo killall -s " + std::to_string(signal) + " gnb";
    my_logger_g.logger->debug("Sent command: {}", command);
    if (system(command.c_str()) != 0) {
        my_logger_g.logger->warn("{}: killall failed", get_name());
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (is_alive()) {
        my_logger_g.logger->error("{} is still alive", get_name());
        return false;
    }
    my_logger_g.logger->debug("Successfully killed {}", get_name());
    return true;
}

bool SRSGNB_Base::restart()
{
    if (!stop()) {
        my_logger_g.logger->error("Failed to stop {}", get_name());
        return false;
    }
    
    if (!start()) {
        my_logger_g.logger->error("Failed to start {}", get_name());
        return false; 
    }
    return true;
}

bool SRSGNB_Base::is_alive()
{
    if (system("sudo killall -s 0 gnb > /dev/null 2>&1") != 0) {
        return false;
    }
    return true;
}
