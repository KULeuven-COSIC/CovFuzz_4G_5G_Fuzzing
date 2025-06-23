#pragma once

#include "Base_Station/srsGNB_base.h"

#include <string>

/* Implementation of the base-station using srsENB with virtual ZMQ radios */
class SRSGNB_ZMQ final : public SRSGNB_Base {
public:
    bool start() override;
    const std::string& get_name() const override;

private:
    const std::string name_ = "srsGNB_ZMQ";
};