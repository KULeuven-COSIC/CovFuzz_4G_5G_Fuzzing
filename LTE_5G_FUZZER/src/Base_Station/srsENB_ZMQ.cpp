#include "Base_Station/srsENB_ZMQ.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Settings/srsran_config.h"
#include "helpers.h"
#include "my_logger.h"

#include <cstdlib>
#include <filesystem>

extern Main_Config main_config_g;
extern My_Logger my_logger_g;
extern SRSRAN_Config srsran_config_g;

bool SRSENB_ZMQ::start()
{
    std::filesystem::path asan_log_dir_path = std::filesystem::path(main_config_g.asan_log_path) / std::filesystem::path(get_name());
    if (!helpers::create_directories_if_not_exist(asan_log_dir_path)) {
        my_logger_g.logger->error("Failed to create directory for ASAN: {}", asan_log_dir_path);
        return false;
    }
    std::string command;
    if (main_config_g.use_monitor) {
        command = "gnome-terminal --geometry=80x24+150+550 --title=srsENB -- bash -c \"sudo ASAN_OPTIONS='log_path=" + 
        asan_log_dir_path.string() + "/enb_asan_" + helpers::get_current_time_stamp_short() + ".log' " + srsran_config_g.path_to_srsran_4G + 
        "/srsenb/src/srsenb --rf.device_name=zmq --rf.device_args='fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2001,id=enb,base_srate=23.04e6;'" + 
        (srsran_config_g.close_base_station_after_fuzzing ? "" : ";bash") + "\"";
    } else {
        command = "sudo ASAN_OPTIONS='log_path=" + asan_log_dir_path.string() + "/enb_asan_" + 
        helpers::get_current_time_stamp_short() + ".log' " + srsran_config_g.path_to_srsran_4G + 
        "/srsenb/src/srsenb --rf.device_name=zmq --rf.device_args='fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2001,id=enb,base_srate=23.04e6;' > /dev/null 2>&1 &";
    }
    
    if (system(command.c_str()) != 0) {
        return false;
    }
    return true;
}

const std::string& SRSENB_ZMQ::get_name() const
{
    return name_;
}