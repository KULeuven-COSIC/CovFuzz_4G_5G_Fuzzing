#include "Fuzzers/fuzzer_factory.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"

#include "Fuzzers/base_fuzzer.h"
#include "Fuzzers/dummy_fuzzer.h"
#include "Fuzzers/random_fuzzer.h"

#include <memory>
#include <stdexcept>

std::unique_ptr<Base_Fuzzer> Fuzzer_Factory::get_fuzzer_by_fuzzing_strategy(FuzzingStrategy fs) {
    switch (fs) {
        case FuzzingStrategy::NONE: return std::make_unique<DummyFuzzer>();
        case FuzzingStrategy::RANDOM_FUZZING: return std::make_unique<RandomFuzzer>();
        default: break;
    }
    throw std::runtime_error("Unknown fuzzing strategy");
}