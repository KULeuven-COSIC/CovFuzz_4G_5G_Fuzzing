#pragma once

#include <string>

#include "zmq_client.h"

class Coverage_Requester {
public:
    bool init(const std::string& server_address);
    bool get_code_coverage_map(size_t map_size, uint8_t* map_ptr);
    uint32_t get_code_coverage_map_size();
    bool reset_code_coverage_map();
    void end_communication();

private:
    ZMQ_Client zmq_client;
    std::string server_address;

    const std::string reset_coverage_map_cmd = "RESET_COVERAGE_MAP";
    const std::string get_coverage_map_cmd = "GET_COVERAGE_MAP";
    const std::string get_coverage_map_size_cmd = "GET_COVERAGE_MAP_SIZE";
    const std::string end_cmd = "OVER";
};