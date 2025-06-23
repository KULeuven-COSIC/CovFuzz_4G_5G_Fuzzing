#pragma once

#include "Base_Station/srsENB_base.h"

#include <string>

/* Implementation of the base-station using srsENB with virtual ZMQ radios */
class SRSENB final : public SRSENB_Base {
public:
    bool start() override;
    const std::string& get_name() const override;

private:
    const std::string name_ = "srsENB";
};