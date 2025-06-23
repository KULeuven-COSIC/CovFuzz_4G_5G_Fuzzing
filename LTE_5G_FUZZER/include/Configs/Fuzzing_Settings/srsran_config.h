#pragma once

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

class SRSRAN_Config
{
public:
    std::string path_to_srsran_4G = "";
    std::string path_to_srsran_project = "";
    std::string path_to_srsran_project_config = "";
    std::string path_to_open5gs = "";
    bool close_base_station_after_fuzzing = true;
    bool close_ue_after_fuzzing = true;
    bool close_core_network_after_fuzzing = true;

    std::ostream& dump (std::ostream& os) const;
};

std::ostream& operator<< (std::ostream& os, const SRSRAN_Config& config);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SRSRAN_Config, path_to_srsran_4G, path_to_srsran_project, path_to_srsran_project_config, path_to_open5gs,
    close_base_station_after_fuzzing, close_ue_after_fuzzing, close_core_network_after_fuzzing)
