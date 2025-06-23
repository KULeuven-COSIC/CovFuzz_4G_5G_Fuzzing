#include "helpers.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Configs/Fuzzing_Settings/main_config.h"

#include "shm/fuzz_config.h"
#include "common_utils.h"

#include "Fuzzers/base_fuzzer.h"
#include "Fuzzers/dummy_fuzzer.h"
#include "Fuzzers/random_fuzzer.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <random>

extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern Fuzz_Config fuzz_config_g;
extern Main_Config main_config_g;

namespace helpers {

bool check_if_path_exists(const std::filesystem::path &path)
{
    try {
        if (!std::filesystem::exists(path)) return false;
    } catch (std::exception& ex) {
        std::cerr << "Caught exception during check_if_path_exists(): " << ex.what() << std::endl;
        return false;
    }
    return true;
}

bool create_directories_if_not_exist(const std::filesystem::path& path) {
    try {
        /* First check if the directories exist */
        if (check_if_path_exists(path)) return true;
        /* If doesn't exist, try to create the directory */
        if (!std::filesystem::create_directories(path)) return false;
    } catch(std::exception& ex) {
        std::cerr << "Caught exception during create_directories_if_not_exist(): " << ex.what() << std::endl;
        return false;
    }
    return true;
}

uint64_t EXP0(uint64_t limit, double lambda) {
	static std::random_device rd;
	std::exponential_distribution<double> distribution(lambda);
	uint64_t rn;
	do {
		rn = (uint64_t)distribution(rd);
	} while (rn >= limit);
	return rn;
}

double URD(double start, double end) {
	static std::random_device rd;
	std::uniform_real_distribution<> distribution(start, end);
	return distribution(rd);
}

uint64_t UR0(uint64_t limit) {
	//if (limit == 0) return 0;
	if (limit == 0) throw std::runtime_error("UR0(0)");
	if (limit == 1) return 0;
	static std::random_device rd;
	//std::default_random_engine generator(rd());
	//generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<long long unsigned> distribution(0,limit-1);
	return (uint64_t) distribution(rd);
}

uint64_t UR1(uint64_t limit) {
	if (limit == 1) return 0;
	static std::random_device rd;
	// std::default_random_engine generator(rd());
	// generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<long long unsigned> distribution(1,limit);
	return (uint64_t) distribution(rd);
}

const std::string get_layer_name_by_idx(uint8_t idx) {
	if (idx == SHM_MUTEX_MAC) return "MAC";
	else if (idx == SHM_MUTEX_RRC) return "RRC";
	return "UNKNOWN";
}

std::string shorten_dissector_summary(const std::string& dissector_summary) {
    std::string result = "";
    bool in_brackets = false;
    for (char c : dissector_summary) {
      if (in_brackets) {
        if (c == ')') {
          in_brackets = false;
          continue;
        }
      } else {
        if (c == '|') break;
        if (c == '(') {
          in_brackets = true;
        } else if (c == ' ') {
          continue;
        } else {
          result += c;
        }
      }
    }
    return result;
}

bool is_state_being_fuzzed(const std::string& state_name) {
    if (!fuzz_strategy_config_g.states_to_fuzz.empty()) {
        return std::find(fuzz_strategy_config_g.states_to_fuzz.begin(), fuzz_strategy_config_g.states_to_fuzz.end(), state_name) != fuzz_strategy_config_g.states_to_fuzz.end();
    }
    if (!fuzz_strategy_config_g.states_not_to_fuzz.empty()) {
        return std::find(fuzz_strategy_config_g.states_not_to_fuzz.begin(), fuzz_strategy_config_g.states_not_to_fuzz.end(), state_name) == fuzz_strategy_config_g.states_not_to_fuzz.end();
    }
    return true;
}

bool is_fuzzing_direction(uint8_t direction)
{
    return (direction == DIRECTION_UPLINK && fuzz_config_g.fuzz_ul()) || 
		(direction == DIRECTION_DOWNLINK && fuzz_config_g.fuzz_dl());
}

bool is_direction_logged(uint8_t direction) {
    if ((main_config_g.log_DL_packets && direction == DIRECTION_DOWNLINK) ||
        (main_config_g.log_UL_packets && direction == DIRECTION_UPLINK)) {
        return true;
    }
    return false;
}

std::string get_direction_by_idx(uint8_t direction) {
  if (direction == DIRECTION_UPLINK) return "UPLINK";
  else if (direction == DIRECTION_DOWNLINK) return "DOWNLINK";
  throw std::runtime_error("Unknown direction idx");
}

}