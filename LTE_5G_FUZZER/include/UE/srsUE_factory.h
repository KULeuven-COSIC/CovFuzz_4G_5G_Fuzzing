#pragma once

#include "UE/srsUE_base.h"
#include "Protocol_Stack/protocol_stack_names.h"

#include <memory>

class SRSUE_Factory {
public:
    static std::unique_ptr<SRSUE_Base> get_ue_by_protocol_name(PROTOCOL_STACK_NAME);
};
