#pragma once

#include "UE/srsUE_base.h"

class SRSUE final : public SRSUE_Base {
public:
    bool start() override;
    const std::string& get_name() const override;

private:
    const std::string name_ = "srsUE";

};