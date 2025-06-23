#pragma once

#include "UE/UE.h"

#include "UE/Communication/AT_Communication.h"

#include <memory>

/* Class implementing the modem with exposed AT command interface */
class AT_Modem : public UE {
public:
    AT_Modem();
    virtual ~AT_Modem() = default;
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
    virtual AT_Response get_functionality();

    std::unique_ptr<AT_Communication> dut_communication_;
    const std::string name_ = "AT_Modem";
};