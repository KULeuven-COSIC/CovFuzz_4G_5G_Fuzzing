#include "Coordinators/dummy_coordinator.h"

#include "my_logger.h"

extern My_Logger my_logger_g;

bool Dummy_Coordinator::init(const std::vector<std::string> &fuzz_strategy_config_names)
{
    if (!init_fuzzing_strategies(fuzz_strategy_config_names)) {
        my_logger_g.logger->error("Failed to init the fuzzing strategies");
        return false;
    }
    return true;
}

void Dummy_Coordinator::deinit()
{
    return;
}

void Dummy_Coordinator::thread_dut_communication_func()
{
    if (!fuzz_strategy_configs_.empty()) {
        if (!prepare_new_fuzzing_sprint(fuzz_strategy_configs_.at(0))) {
            my_logger_g.logger->error("Failed to prepare new fuzzing sprint");
        }
    }

    return;
}

bool Dummy_Coordinator::reset_target()
{
    return true;
}

std::string Dummy_Coordinator::get_name()
{
    return name_;
}
