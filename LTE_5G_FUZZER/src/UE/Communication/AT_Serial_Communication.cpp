#include "UE/Communication/AT_Serial_Communication.h"

#include "Configs/Fuzzing_Settings/serial_config.h"

#include "my_logger.h"

extern My_Logger my_logger_g;
extern Serial_Config serial_config_g;

bool AT_Serial_Communication::init(const std::string ue_name) {
    bool port_opened = false;
    for (const std::string& port : serial_config_g.modem_ports) {
        try {
            serial_port.Open(port);
            port_opened = true;
            my_logger_g.logger->info("Using serial port {}", port);
            break;
        } catch(std::exception& ex) {
            my_logger_g.logger->warn("Port {} is not available: {}", port, ex.what());
            continue;
        }
    }
    if (!port_opened) {
        my_logger_g.logger->error("None of the ports can be open!");
        return false; //throw std::runtime_error("Serial port could not be opened");
    }
    try {
        LibSerial::BaudRate baudrate = get_libserial_baudrate(serial_config_g.baudrate);
        if (baudrate == LibSerial::BaudRate::BAUD_INVALID) {
            my_logger_g.logger->error("Invalid baudrate: {}", serial_config_g.baudrate);
            return false;
        }
        serial_port.SetBaudRate(baudrate);
    } catch (std::exception& ex) {
        my_logger_g.logger->error("Exception happened in LibSerial: {}", ex.what());
        return false;
    }
    return true;
}

void AT_Serial_Communication::deinit() {
    try {
        serial_port.Close();
    } catch (std::exception& ex) {
        my_logger_g.logger->error("Exception happened in LibSerial: {}", ex.what());
    }
}

bool AT_Serial_Communication::restart() {
    return true;
}

bool AT_Serial_Communication::wait_for_a_string(const std::string& expected_string, size_t timer) {
    // Wait until the serial port is open
    size_t seconds = 40;
    while (!serial_port.IsOpen()) {
        my_logger_g.logger->warn("Serial port is closed");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (--seconds == 0) {
            my_logger_g.logger->error("Could not open a serial port");
            return false;
        }
    }

    // Wait for the expected
    size_t sleep_time_ms = 10;
    size_t rounds_left = timer * 1000 / sleep_time_ms;
    std::string serial_output;
    char tmp;
    do {
        try {
            serial_port.ReadByte(tmp, sleep_time_ms);
        } catch(std::exception& ex) {
            // my_logger_g.logger->error("Exception happened in LibSerial ReadByte: {}", ex.what());
            // return "";
        }
        serial_output += tmp;
    } while ((--rounds_left) && ((expected_string.empty() || serial_output.find(expected_string) == std::string::npos)));
    if (rounds_left == 0) return false;
    return true;
}

AT_Response AT_Serial_Communication::send_AT_COMMAND_blocking(const std::string& at_command) {
    std::string serial_output = send_command_blocking(at_command, OK);
    return AT_Response(at_command, serial_output);
}

AT_Response AT_Serial_Communication::send_AT_COMMAND_nonblocking(const std::string& at_command, size_t timer, const std::string expected_output) {
    
    std::string serial_output = send_command_nonblocking(at_command, timer, expected_output);
    if (serial_output.empty()) return AT_Response(at_command, "TIMEOUT");
    return AT_Response(at_command, serial_output);
}

std::string AT_Serial_Communication::send_command_nonblocking(const std::string& command, size_t timer, const std::string& expected_output, std::string command_ends_with) {
    size_t sleep_time_ms = 10;
    size_t rounds_left = timer * 1000 / sleep_time_ms;
    my_logger_g.logger->debug("Sent non-blocking command: {}", command);
    if (!serial_port.IsOpen()) {
        my_logger_g.logger->warn("Serial port is closed");
        return "";
    }
    my_logger_g.logger->debug("Writing the serial command");
    bool write_success = false;
    for (int i = 0; i < 3; i++) {
        try {
            serial_port.Write(command + command_ends_with);
            serial_port.DrainWriteBuffer();
            write_success = true;
            break;
        } catch(std::exception& ex) {
            my_logger_g.logger->warn("Exception happened during serial write: {}", ex.what());
            continue;
        }
    }
    if (!write_success) throw std::runtime_error("Serial write failed");

    std::string serial_output;
    char tmp;
    do {
        try {
            serial_port.ReadByte(tmp, sleep_time_ms);
        } catch(std::exception& ex) {
            // my_logger_g.logger->error("Exception happened in LibSerial ReadByte: {}", ex.what());
            // return "";
        }
        serial_output += tmp;
    } while ((--rounds_left) && ((expected_output.empty() || serial_output.find(expected_output) == std::string::npos)));

    boost::algorithm::trim(serial_output);
    my_logger_g.logger->debug("Got response: {}", serial_output);
    my_logger_g.logger->flush();
    if (rounds_left == 0 && !expected_output.empty()) {
        my_logger_g.logger->warn("Returning nothing");
        return "";
    }
    return serial_output;   
}

std::string AT_Serial_Communication::send_command_blocking(const std::string& command, const std::string& expected_output, std::string command_ends_with) {
    size_t sleep_time_ms = 100;
    my_logger_g.logger->debug("Sent blocking command: {}", command);
    serial_port.Write(command + command_ends_with);
    serial_port.DrainWriteBuffer();

    std::string serial_output;
    char tmp;
    do {
        try {
            serial_port.ReadByte(tmp, sleep_time_ms);
        } catch(std::exception& ex) {
            // my_logger_g.logger->error("Exception happened in LibSerial ReadByte: {}", ex.what());
            // return "";
        }
        serial_output += tmp;
    } while ((expected_output.empty() || serial_output.find(expected_output) == std::string::npos) && (!expected_output.empty() || serial_output.empty()));

    boost::algorithm::trim(serial_output);
    my_logger_g.logger->debug("Got response: {}", serial_output);
    my_logger_g.logger->flush();
    return serial_output;   
}

LibSerial::BaudRate get_libserial_baudrate(const std::string& baudrate) {
    if (baudrate == "Default") return LibSerial::BaudRate::BAUD_DEFAULT;
    else if (baudrate == "50") return LibSerial::BaudRate::BAUD_50;
    else if (baudrate == "75") return LibSerial::BaudRate::BAUD_75;
    else if (baudrate == "110") return LibSerial::BaudRate::BAUD_110;
    else if (baudrate == "134") return LibSerial::BaudRate::BAUD_134;
    else if (baudrate == "150") return LibSerial::BaudRate::BAUD_150;
    else if (baudrate == "200") return LibSerial::BaudRate::BAUD_200;
    else if (baudrate == "300") return LibSerial::BaudRate::BAUD_300;
    else if (baudrate == "600") return LibSerial::BaudRate::BAUD_600;
    else if (baudrate == "1200") return LibSerial::BaudRate::BAUD_1200;
    else if (baudrate == "1800") return LibSerial::BaudRate::BAUD_1800;
    else if (baudrate == "2400") return LibSerial::BaudRate::BAUD_2400;
    else if (baudrate == "4800") return LibSerial::BaudRate::BAUD_4800;
    else if (baudrate == "9600") return LibSerial::BaudRate::BAUD_9600;
    else if (baudrate == "19200") return LibSerial::BaudRate::BAUD_19200;
    else if (baudrate == "38400") return LibSerial::BaudRate::BAUD_38400;
    else if (baudrate == "57600") return LibSerial::BaudRate::BAUD_57600;
    else if (baudrate == "115200") return LibSerial::BaudRate::BAUD_115200;
    else if (baudrate == "230400") return LibSerial::BaudRate::BAUD_230400;
    else return LibSerial::BaudRate::BAUD_INVALID;
}