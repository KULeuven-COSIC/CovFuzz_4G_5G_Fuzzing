#pragma once

#include "Coordinators/base_coordinator.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Communication/srsue_communication.h"

#include <vector>

class SRS_Coordinator final : public Base_Coordinator {
public:

    SRS_Coordinator();

    void thread_dut_communication_func() override;
    bool init(const std::vector<std::string>& fuzz_strategy_config_names) override;
    void deinit() override;
    bool reset_target() override;

    void restart_fuzzing_process();

    std::string get_name() override;

private:
    srsue_communication srsue_comm;
    std::string name_ = "SRS Coordinator";
};
