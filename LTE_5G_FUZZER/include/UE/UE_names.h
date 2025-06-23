#pragma once

#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

enum class UE_Name {
    AT_Modem,
    Android_UE,
    DUMMY_UE
};

inline std::string get_UE_name_by_idx(UE_Name ue) {
    switch(ue) {
        case UE_Name::AT_Modem:                   return "AT_Modem";
        case UE_Name::Android_UE:                 return "Android_UE";
        case UE_Name::DUMMY_UE:                   return "DUMMY_UE";
    }
    throw std::runtime_error("Unknown UE");
}

NLOHMANN_JSON_SERIALIZE_ENUM( UE_Name, {
    {UE_Name::AT_Modem, "AT_Modem"},
    {UE_Name::Android_UE, "Android_UE"},
    {UE_Name::DUMMY_UE, "DUMMY_UE"}
})