#pragma once

#include "UE/UE.h"

#include <memory>

/* Class implementing the modem with exposed AT command interface */
class Android_UE : public UE {
public:
    Android_UE() = default;
    virtual ~Android_UE() = default;
    virtual bool start() override;
    virtual bool stop() override;
    virtual bool restart() override;
    
    virtual bool connect_to_network() override;
    virtual bool disconnect_from_network() override;
    virtual bool is_connected() override;
    virtual bool is_alive() override;

protected:
    virtual bool set_to_airplane_mode();
    virtual bool set_ue_to_full_functionality();

    const std::string name_ = "Android_UE";
};