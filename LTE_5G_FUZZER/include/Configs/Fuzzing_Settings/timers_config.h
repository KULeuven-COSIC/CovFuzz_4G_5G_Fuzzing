#pragma once

#include <iostream>

#include <nlohmann/json.hpp>

class Timers_Config
{
public:
    size_t AT_CONNECT_TO_NETWORK = 1;
    size_t AT_FLIGHT_MODE = 3;
    size_t AT_SET_FULL_FUNCTIONALITY = 3;
    size_t AT_SET_TO_MIN_FUNCTIONALITY = 15;
    size_t AT_SEARCH_AVAILABLE_NETWORKS = 360;
    size_t AT_AT = 3;

public:
    std::ostream& dump(std::ostream& os) const;
};

std::ostream& operator<< (std::ostream& os, const Timers_Config& path_config);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Timers_Config, AT_CONNECT_TO_NETWORK, AT_FLIGHT_MODE, AT_SET_FULL_FUNCTIONALITY, 
                                                                AT_SET_TO_MIN_FUNCTIONALITY, AT_SEARCH_AVAILABLE_NETWORKS, AT_AT)
