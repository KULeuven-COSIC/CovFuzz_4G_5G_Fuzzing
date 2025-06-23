#pragma once

#include "Coordinators/base_coordinator.h"

#include <string>

class Dummy_Coordinator final : public Base_Coordinator {
public:
    bool init(const std::vector<std::string>& fuzz_strategy_config_names) override;
    void deinit() override;
    void thread_dut_communication_func() override;
    bool reset_target() override;

    std::string get_name() override;
private:
    std::string name_ = "Dummy Coordinator";
};