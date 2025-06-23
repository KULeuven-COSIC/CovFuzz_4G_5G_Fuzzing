#pragma once

#include "UE/UE.h"

/* Fake UE class */
class Dummy_UE final : public UE {
public:
    bool start() override;
    bool stop() override;
    bool restart() override;
    bool connect_to_network() override;
    bool disconnect_from_network() override;
    bool is_connected() override;
    bool is_alive() override;
};