#pragma once

// #include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
// #include "Fuzzers/base_fuzzer.h"
#include "common_utils.h"
#include "packet.h"
#include "Protocol_Stack/Protocol_type.h"
 

// #include "Fuzzers/base_fuzzer.h"

#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

/* Overloading the << operator to print the vectors */
template <typename T>
inline std::ostream& operator<< (std::ostream& os, const std::vector<T>& v) {
    for (const T& t : v) {
        os << t << " ";
    }
    return os;
}

/* DEFINE DIFFERENT COLORS */
#define BLACK  "\033[0;30m"
#define RED    "\033[0;31m"
#define GREEN  "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE   "\033[0;34m"
#define CRESET "\033[0m"

#define MODEM_DEAD "Modem is not alive"

NLOHMANN_JSON_NAMESPACE_BEGIN
template <typename T>
struct adl_serializer<std::shared_ptr<T>> {
    static void to_json(json& j, const std::shared_ptr<T>& ptr) {
        j = *ptr;
    }

    static void from_json(const json& j, std::shared_ptr<T>& ptr) {
        ptr = std::make_shared<T>(j.get<T>());
    }
};
NLOHMANN_JSON_NAMESPACE_END

namespace helpers {

template <typename C>
inline bool parse_json_file_into_class_instance(const std::filesystem::path& json_file_path, C& class_instance) {
    std::ifstream json_file(json_file_path);
    if (!json_file.is_open()) {
        std::cerr << "Could not open the file: " << json_file_path << std::endl;
        return false;
    }
    nlohmann::json json_obj;
    try {
        json_file >> json_obj;
        class_instance = json_obj.get<C>();
    } catch(std::exception& ex) {
        std::cerr << "Caught an exception during json parsing: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

template <typename C>
inline bool write_class_instance_to_json_file(C& class_instance, const std::filesystem::path& json_file_path) {
    std::ofstream json_file(json_file_path);
    if (!json_file.is_open()) {
        std::cerr << "Could not open the file: " << json_file_path << std::endl;
        return false;
    }
    try {
        nlohmann::json j = class_instance;
        json_file << std::setw(4) << j;
    } catch(std::exception& ex) {
        std::cerr << "Caught an exception during json loading: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

bool check_if_path_exists(const std::filesystem::path& path);
bool create_directories_if_not_exist(const std::filesystem::path& path);

inline std::string get_current_time_stamp() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char str[60];
    sprintf(str, "%d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(str);
}

inline std::string get_current_time_stamp_short() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char str[60];
    sprintf(str, "%d-%02d-%02d_%02d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(str);
}

uint64_t EXP0(uint64_t limit, double lambda = 0.3); // Generate random number in range [0, limit) following exponential distribution
double URD(double start=0.0, double end=1.0);
uint64_t UR0(uint64_t limit); // Generate random number in range [0, limit)
uint64_t UR1(uint64_t limit); // Generate random number in range [1, limit]

const std::string get_layer_name_by_idx(uint8_t idx);


std::string shorten_dissector_summary(const std::string& dissector_summary);

bool is_state_being_fuzzed(const std::string& state_name);

bool is_fuzzing_direction(uint8_t direction);

inline static std::string get_dissector_by_4G_channel(CHANNEL channel) {
	std::string dissector;
	switch (channel) {
    case 0: // RRC BCCH-BCH
        dissector = "lte-rrc.bcch.bch";
        break;
    case 1: // RRC BCCH-DL-SCH
        dissector = "lte-rrc.bcch.dl.sch";
        break;
    case 2: // RRC PCCH
        dissector = "lte-rrc.pcch";
        break;
    case 3: // RRC DL CCCH
        dissector = "lte-rrc.dl.ccch";
        break;
    case 4: // RRC UL CCCH
        dissector = "lte-rrc.ul.ccch";
        break;
    case 5: // RRC DL DCCH
		    dissector = "lte-rrc.dl.dcch";
		    break;
    case 6: // RRC UL DCCH
        dissector = "lte-rrc.ul.dcch";
		    break;
    default:
        dissector = "";
  }
	return dissector;
}

inline static std::string get_dissector_by_5G_channel(CHANNEL channel) {
	std::string dissector;
	switch (channel) {
    case 0: // RRC BCCH-BCH
        dissector = "nr-rrc.bcch.bch";
        break;
    case 1: // RRC BCCH-DL-SCH
        dissector = "nr-rrc.bcch.dl.sch";
        break;
    case 2: // RRC PCCH
        dissector = "nr-rrc.pcch";
        break;
    case 3: // RRC DL CCCH
        dissector = "nr-rrc.dl.ccch";
        break;
    case 4: // RRC UL CCCH
        dissector = "nr-rrc.ul.ccch";
        break;
    case 5: // RRC DL DCCH
		    dissector = "nr-rrc.dl.dcch";
		    break;
    case 6: // RRC UL DCCH
        dissector = "nr-rrc.ul.dcch";
		    break;
    default:
        dissector = "";
  }
	return dissector;
}

inline static std::string get_RRC_dissector_by_channel(CHANNEL channel, PROTOCOL_TYPE t) {
    if (t == PROTOCOL_TYPE::PROTO_4G) return get_dissector_by_4G_channel(channel);
    else if (t == PROTOCOL_TYPE::PROTO_5G) return get_dissector_by_5G_channel(channel);
    throw std::runtime_error("Cannot get RRC dissector: Unsupported protocol");
}

inline static std::string get_MAC_dissector(PROTOCOL_TYPE t) {
    if (t == PROTOCOL_TYPE::PROTO_4G) return "mac-lte-framed";
    else if (t == PROTOCOL_TYPE::PROTO_5G) return "mac-nr-framed";
    throw std::runtime_error("Cannot get MAC dissector: Unsupported protocol");
}

inline void clear_screen() {
    if (system("clear") == -1) {
        throw std::runtime_error("Command failed: clear");
    }
}

bool is_direction_logged(uint8_t direction);
std::string get_direction_by_idx(uint8_t direction);

inline Packet get_sample_packet() {

  uint8_t pdu[] = {0x41, 0xA3, 0x97, 0xE5, 0xA1, 0xF8};
  size_t pdu_size = 6;
  size_t pdu_type_ = 0; // Not important
  size_t direction = 0;
  size_t layer_num = 0;

  auto p = Packet(pdu, pdu_size, pdu_type_, direction, layer_num);
  p.set_dissector_name("lte_rrc.ul_ccch");

  return p;
}

// std::unique_ptr<Base_Fuzzer> get_fuzzer_instance_by_fuzzing_strategy(FuzzingStrategy fs);

}