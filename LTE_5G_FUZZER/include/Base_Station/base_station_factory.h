#pragma once

#include "Base_Station/base_station.h"
#include "Protocol_Stack/protocol_stack_names.h"

#include <memory>

class Base_Station_Factory {
public:
    static std::unique_ptr<Base_Station> get_base_station_by_protocol_name(PROTOCOL_STACK_NAME);
};