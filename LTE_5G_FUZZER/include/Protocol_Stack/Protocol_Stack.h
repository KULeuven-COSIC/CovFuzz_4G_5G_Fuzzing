#pragma once

#include "Protocol_Stack/Protocol_Stack_Base.h"

#include "Base_Station/base_station.h"
#include "Core_Network/core_network.h"

#include <memory>

/* Base class for the protocol stack containing a base station and a core network */
class Protocol_Stack : public Protocol_Stack_Base {
public:
    Protocol_Stack();
    virtual ~Protocol_Stack() = default;
    virtual bool start();
    virtual bool stop();
    virtual bool restart();
    virtual bool is_running();

protected:
    std::unique_ptr<Base_Station> base_station_;
    std::unique_ptr<Core_Network> core_network_;
};