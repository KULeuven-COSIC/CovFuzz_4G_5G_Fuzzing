#include "Configs/Fuzzing_Settings/main_config.h"

#include "helpers.h"

#include <iostream>

Main_Config main_config_g;

std::ostream& Main_Config::dump(std::ostream& os) const {
    os << "Target name: " << target_name << std::endl;
    os << "Network names: " << network_names << std::endl;
    os << "asan_log_path: " << asan_log_path << std::endl;
    os << "Use monitor: " << std::boolalpha << use_monitor << std::endl;
    os << "Log DL packets: " << std::boolalpha << log_DL_packets << std::endl;
    os << "Log UL packets: " << std::boolalpha << log_UL_packets << std::endl;
    os << "Path config path: " << srsran_config_path << std::endl;
    os << "Timers config path: " << timers_config_path << std::endl;
    os << "Serial config path: " << serial_config_path << std::endl;
    os << "Verbose: " << std::boolalpha << verbose << std::endl;
    return os;
}

std::ostream& operator<< (std::ostream& os, const Main_Config& config) {
    return config.dump(os);
}

