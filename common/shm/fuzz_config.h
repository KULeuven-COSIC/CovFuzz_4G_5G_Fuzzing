#pragma once

#include <nlohmann/json.hpp>

#define FUZZ_CONFIG_PATH "FUZZ_CONFIG_PATH_PLACEHOLDER"

class Fuzz_Config {
public:
    /* Downlink fuzzing */
    bool FUZZ_RRC_DL = false;
    bool FUZZ_MAC_DL = false;
    bool FUZZ_MIB = false;
    bool FUZZ_SIB = false;

    /* Uplink fuzzing */
    bool FUZZ_RRC_UL = false;
    bool FUZZ_MAC_UL = false;

public:
    bool fuzz_ul() const;
    bool fuzz_dl() const;
};

bool parse_fuzz_config(const std::string& path_to_config = FUZZ_CONFIG_PATH);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Fuzz_Config, FUZZ_RRC_DL, FUZZ_MAC_DL, FUZZ_MIB, FUZZ_SIB,
            FUZZ_RRC_UL, FUZZ_MAC_UL)