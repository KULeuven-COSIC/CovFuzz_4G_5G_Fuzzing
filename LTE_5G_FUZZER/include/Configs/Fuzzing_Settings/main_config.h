#pragma once

#include "UE/UE_names.h"
#include "Protocol_Stack/protocol_stack_names.h"

#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

class Main_Config
{
public:
    PROTOCOL_STACK_NAME protocol_stack_name;
    UE_Name ue_name;
    std::string asan_log_path = "/tmp";
    std::string target_name = "UNKNOWN_TARGET"; // User-defined name of the target. Is only used to create log/<target_name> directory.
    std::vector<std::string> network_names = {"00101"};
    bool use_monitor = true;
    bool log_DL_packets = true;
    bool log_UL_packets = true;
    std::string timers_config_path = "";
    std::string serial_config_path = "";
    std::string srsran_config_path = "";
    bool verbose = false;

public:
    std::ostream& dump(std::ostream& os) const;
};

std::ostream& operator<< (std::ostream& os, const Main_Config& path_config);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Main_Config, protocol_stack_name, ue_name, target_name, asan_log_path, use_monitor,
                                                                log_DL_packets, log_UL_packets, srsran_config_path, serial_config_path, 
                                                                timers_config_path, verbose)

