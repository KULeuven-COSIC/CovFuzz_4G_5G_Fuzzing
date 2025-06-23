#pragma once

#include "Core_Network/core_network.h"
#include "Protocol_Stack/protocol_stack_names.h"

#include <memory>

class Core_Network_Factory {
public:
    static std::unique_ptr<Core_Network> get_core_network_by_protocol_name(PROTOCOL_STACK_NAME);
};