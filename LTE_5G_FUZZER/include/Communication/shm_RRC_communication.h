#pragma once

#include "Communication/shm_layer_communication.h"

#include "packet.h"

class SHM_RRC_Communication final : public SHM_Layer_Communication {
public:
    SHM_RRC_Communication();
    void send(Packet& packet) override;
    Packet receive() override;
};