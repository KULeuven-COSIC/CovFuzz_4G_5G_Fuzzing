#pragma once

#include "Core_Network/core_network.h"

#include <string>

class Open5Gs final : public Core_Network {
public:
    bool start() override;
    bool stop() override;
    bool restart() override;
    bool is_alive() override;

private:
    const std::string name_ = "Open5Gs";
};