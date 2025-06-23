#pragma once

#include "UE/Communication/AT_Communication.h"
#include "UE/Communication/AT_Serial_Communication.h"

#include "libserial/SerialPort.h"
#include <nlohmann/json.hpp>

#include <string>

class AT_Serial_Communication : public AT_Communication {
public:

    bool init(const std::string ue_name) override;
    bool restart() override;
    void deinit() override;

    bool wait_for_a_string(const std::string& expected_string, size_t timer) override;
    AT_Response send_AT_COMMAND_blocking(const std::string& at_command) override;
    AT_Response send_AT_COMMAND_nonblocking(const std::string& at_command, size_t timer, const std::string expected_output = "") override;
    std::string send_command_nonblocking(const std::string& command, size_t timer, const std::string& expected_output, std::string command_ends_with = "\r\n") override;
    std::string send_command_blocking(const std::string& command, const std::string& expected_output, std::string command_ends_with = "\r\n") override;

private:
    LibSerial::SerialPort serial_port;
};

LibSerial::BaudRate get_libserial_baudrate(const std::string& baudrate);