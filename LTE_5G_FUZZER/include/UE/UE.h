#pragma once

// Abstract class describing COTS_UE's functionalities
class UE {
public:
    virtual ~UE() = default;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool restart() = 0;
    virtual bool connect_to_network() = 0;
    virtual bool disconnect_from_network() = 0;
    virtual bool is_connected() = 0;
    virtual bool is_alive() = 0;
};
