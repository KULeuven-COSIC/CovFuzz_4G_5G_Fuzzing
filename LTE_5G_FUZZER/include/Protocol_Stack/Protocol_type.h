#pragma once

#include "Protocol_Stack/protocol_stack_names.h"

#include <string>
#include <vector>

// Supported protocols
enum class PROTOCOL_TYPE {
    PROTO_4G,
    PROTO_5G,
    DUMMY
};

std::vector<std::string> get_field_prefixes_by_layer_idx(uint8_t mutex_name, PROTOCOL_TYPE protocol_type);
PROTOCOL_TYPE get_protocol_type_by_protocol_stack_name(PROTOCOL_STACK_NAME);