#pragma once

#include "Coordinators/base_coordinator.h"
#include "Protocol_Stack/protocol_stack_names.h"

#include <memory>

class Coordinator_Factory {
public:
    static std::unique_ptr<Base_Coordinator> get_coordinator_by_protocol_name(PROTOCOL_STACK_NAME);
};
