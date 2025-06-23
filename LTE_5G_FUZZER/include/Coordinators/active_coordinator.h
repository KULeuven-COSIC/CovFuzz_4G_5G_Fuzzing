#pragma once

#include "Coordinators/base_coordinator.h"

class Active_Coordinator final : public Base_Coordinator {
public:

    Active_Coordinator();
    ~Active_Coordinator();

    bool init(const std::vector<std::string>&) override;
    void deinit() override;
    void thread_dut_communication_func() override;
    bool reset_target() override;

    std::string get_name() override;

private:
    const int max_tries = 3;
    std::string name_ = "Active Coordinator";
};