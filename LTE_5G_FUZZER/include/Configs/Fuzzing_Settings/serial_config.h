#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

class Serial_Config {
public:
    std::string baudrate = "Default";
    std::vector<std::string> modem_ports = {""};

    std::ostream& dump (std::ostream& os) const;
};

std::ostream& operator<< (std::ostream& os, const Serial_Config& config);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Serial_Config, baudrate, modem_ports)