#pragma once

#include "UE/Communication/AT_Response.h"

#include <string>

class AT_Communication {
public:
    virtual ~AT_Communication() {};
    virtual bool init(const std::string ue_name) = 0;
    virtual bool restart() = 0;
    virtual void deinit() = 0;

    virtual bool wait_for_a_string(const std::string& expected_string, size_t timer) = 0;
    virtual AT_Response send_AT_COMMAND_blocking(const std::string& at_command) = 0;
    virtual AT_Response send_AT_COMMAND_nonblocking(const std::string& at_command, size_t timer, const std::string expected_output = "") = 0;
    virtual std::string send_command_nonblocking(const std::string& command, size_t timer, const std::string& expected_output, std::string command_ends_with) = 0;
    virtual std::string send_command_blocking(const std::string& command, const std::string& expected_output, std::string command_ends_with) = 0;

};