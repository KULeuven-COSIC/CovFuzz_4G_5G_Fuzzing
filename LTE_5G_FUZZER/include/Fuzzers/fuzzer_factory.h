#pragma once

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"

#include "Fuzzers/base_fuzzer.h"

#include <memory>

class Fuzzer_Factory {
public:
    static std::unique_ptr<Base_Fuzzer> get_fuzzer_by_fuzzing_strategy(FuzzingStrategy fs);
};