#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"

#include <iostream>
#include <string>

Fuzz_Strategy_Config fuzz_strategy_config_g;

std::string get_fuzzing_strategy_name_by_idx(FuzzingStrategy fs) {
	switch (fs) {
		case FuzzingStrategy::NONE: return "None";
		case FuzzingStrategy::RANDOM_FUZZING: return "RANDOM_FUZZING";
		default: break;
	}
	throw std::runtime_error("Unknown fuzzing strategy");
}

std::string get_all_fuzzing_strategy_names(const std::vector<FuzzingStrategy>& fsv) {
	std::string result = "";
	for (const auto& fs : fsv) {
		result += get_fuzzing_strategy_name_by_idx(fs);
		result += ", ";
	}
	result.pop_back(); result.pop_back();
	return result;
}

std::ostream &Fuzz_Strategy_Config::dump(std::ostream &os) const
{
    os << "Fuzzing strategies: " << get_all_fuzzing_strategy_names(fuzzing_strategies) << std::endl;
    return os;
}