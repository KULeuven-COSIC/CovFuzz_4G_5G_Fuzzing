#pragma once

#include "UE/UE.h"
#include "UE/UE_names.h"

#include <memory>

class UE_Factory {
public:
    static std::unique_ptr<UE> get_ue_by_protocol_name(UE_Name);
};
