#include "Configs/Fuzzing_Settings/timers_config.h"

#include <iostream>

Timers_Config timers_config_g;

std::ostream& Timers_Config::dump (std::ostream& os) const {
    os << "AT_CONNECT_TO_NETWORK: " << AT_CONNECT_TO_NETWORK << std::endl;
    os << "AT_FLIGHT_MODE: " << AT_FLIGHT_MODE << std::endl;
    os << "AT_SET_FULL_FUNCTIONALITY: " << AT_SET_FULL_FUNCTIONALITY << std::endl;
    os << "AT_SET_TO_MIN_FUNCTIONALITY: " << AT_SET_TO_MIN_FUNCTIONALITY << std::endl;
    os << "AT_SEARCH_AVAILABLE_NETWORKS: " << AT_SEARCH_AVAILABLE_NETWORKS << std::endl;
    os << "AT_AT: " << AT_AT << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Timers_Config &config)
{
    return config.dump(os);
}