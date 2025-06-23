#pragma once

/* Abstract class representing a core network */
class Core_Network {
public:
    virtual ~Core_Network() {};
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool restart() = 0;
    virtual bool is_alive() = 0;
};