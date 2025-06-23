#pragma once

#include "Communication/shm_layer_communication.h"

#include "packet.h"

class SHM_MAC_Communication final : public SHM_Layer_Communication {
public:
    SHM_MAC_Communication();
    void send(Packet& packet) override;
    Packet receive() override;
};