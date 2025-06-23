#pragma once

/* Base class for the protocol stack. */
class Protocol_Stack_Base {
public:
    virtual ~Protocol_Stack_Base() = default;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool restart() = 0;
    virtual bool is_running() = 0;
};