#include "Configs/Fuzzing_Settings/srsran_config.h"
#include <iostream>

SRSRAN_Config srsran_config_g;

std::ostream& SRSRAN_Config::dump (std::ostream& os) const {
    os << "path_to_srsran_4G: " << path_to_srsran_4G << std::endl;
    os << "path_to_srsran_project: " << path_to_srsran_project << std::endl;
    os << "path_to_srsran_project_config: " << path_to_srsran_project_config << std::endl;
    os << "close_base_station_after_fuzzing: " << close_base_station_after_fuzzing << std::endl;
    os << "close_ue_after_fuzzing: " << close_ue_after_fuzzing << std::endl;
    os << "close_core_network_after_fuzzing: " << close_core_network_after_fuzzing << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const SRSRAN_Config &config)
{
    return config.dump(os);
}