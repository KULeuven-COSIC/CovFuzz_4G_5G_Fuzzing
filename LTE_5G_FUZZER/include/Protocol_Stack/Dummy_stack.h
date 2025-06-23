#pragma once

#include "Protocol_Stack/Protocol_Stack.h"
#include "Protocol_Stack/Protocol_type.h"

class Dummy_stack : public Protocol_Stack {
public:

    virtual ~Dummy_stack() = default;

    bool start() override;
    bool stop() override;
    bool restart() override;
    bool is_running() override;
    // PROTOCOL_TYPE get_protocol_type() override;

};