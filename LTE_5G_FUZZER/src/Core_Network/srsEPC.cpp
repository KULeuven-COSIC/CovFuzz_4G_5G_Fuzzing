#include "Core_Network/srsEPC.h"

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

bool srsEPC::start()
{
    std::filesystem::path asan_log_dir_path = std::filesystem::path(main_config_g.asan_log_path) / std::filesystem::path(name_);
    if (!helpers::create_directories_if_not_exist(asan_log_dir_path)) {
        my_logger_g.logger->error("Failed to create directory for ASAN: {}", asan_log_dir_path);
        return false;
    }
    std::string command;
    if (main_config_g.use_monitor) {
        command = "gnome-terminal --geometry=80x24+150+50 --title=srsEPC -- bash -c \"sudo ASAN_OPTIONS='log_path=" + 
            asan_log_dir_path.string() + "/epc_asan_" + helpers::get_current_time_stamp_short() + ".log' " + 
            srsran_config_g.path_to_srsran_4G + "/srsepc/src/srsepc" + 
            (srsran_config_g.close_core_network_after_fuzzing ? "" : ";bash") + "\"";
    } else {
        command = "sudo ASAN_OPTIONS='log_path=" + asan_log_dir_path.string() + 
            "/epc_asan_" + helpers::get_current_time_stamp_short() + ".log' " + 
            srsran_config_g.path_to_srsran_4G + "/srsepc/src/srsepc > /dev/null 2>&1 &";
    }

    if (system(command.c_str()) != 0) {
        return false;
    }
    return true;
}

bool srsEPC::stop()
{
    int signal = SIGKILL;
    std::string command = "sudo killall -s " + std::to_string(signal) + " srsepc";
    my_logger_g.logger->debug("Sent command: {}", command);
    if (system(command.c_str()) != 0) {
        my_logger_g.logger->warn("{}: killall failed", name_);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (is_alive()) {
        my_logger_g.logger->error("{} is still alive", name_);
        return false;
    }
    my_logger_g.logger->debug("Successfully killed {}", name_);
    return true;
}

bool srsEPC::restart()
{
    if (!stop()) {
        my_logger_g.logger->error("Failed to stop {}", name_);
        return false;
    }
    
    if (!start()) {
        my_logger_g.logger->error("Failed to start {}", name_);
        return false; 
    }
    return true;
}

bool srsEPC::is_alive()
{
    if (system("sudo killall -s 0 srsepc > /dev/null 2>&1") != 0) {
        return false;
    }
    return true;
}
