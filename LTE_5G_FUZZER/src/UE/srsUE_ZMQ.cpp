#include "UE/srsUE_ZMQ.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Settings/srsran_config.h"
#include "helpers.h"
#include "my_logger.h"

#include <cstdlib>
#include <filesystem>

extern Main_Config main_config_g;
extern My_Logger my_logger_g;
extern SRSRAN_Config srsran_config_g;

bool SRSUE_ZMQ::start()
{
    std::filesystem::path asan_log_dir_path = std::filesystem::path(main_config_g.asan_log_path) / std::filesystem::path(get_name());
    if (!helpers::create_directories_if_not_exist(asan_log_dir_path)) {
        my_logger_g.logger->error("Failed to create directory for ASAN: {}", asan_log_dir_path);
        return false;
    }
    std::string command;
    if (main_config_g.use_monitor) {
        command = "gnome-terminal --geometry=80x24+900+550 --title=DUT -- bash -c \"sudo ASAN_OPTIONS='log_path=" + 
        asan_log_dir_path.string() + "/ue_asan_" + helpers::get_current_time_stamp_short() + ".log' " + srsran_config_g.path_to_srsran_4G + 
        "/srsue/src/srsue --rf.device_name=zmq --rf.device_args='tx_port=tcp://*:2001,rx_port=tcp://localhost:2000,id=ue,base_srate=23.04e6' --gw.netns=ue1" +
        (srsran_config_g.close_ue_after_fuzzing ? "" : ";bash") + "\"";
    } else {
        command = "sudo ASAN_OPTIONS='log_path=" + asan_log_dir_path.string() + "/ue_asan_" +
        helpers::get_current_time_stamp_short() + ".log' " + srsran_config_g.path_to_srsran_4G + 
        "/srsue/src/srsue --rf.device_name=zmq --rf.device_args='tx_port=tcp://*:2001,rx_port=tcp://localhost:2000,id=ue,base_srate=23.04e6' --gw.netns=ue1 > /dev/null 2>&1 &";
    }
    
    if (system(command.c_str()) != 0) {
        return false;
    }
    return true;
}

const std::string& SRSUE_ZMQ::get_name() const
{
    return name_;
}