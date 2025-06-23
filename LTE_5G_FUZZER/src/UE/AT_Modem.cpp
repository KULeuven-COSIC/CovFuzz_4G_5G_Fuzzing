#include "UE/AT_Modem.h"

#include "UE/Communication/AT_Serial_Communication.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Settings/timers_config.h"

#include "helpers.h"
#include "my_logger.h"

#include <chrono>
#include <thread>

extern My_Logger my_logger_g;
extern Main_Config main_config_g;
extern Timers_Config timers_config_g;

AT_Modem::AT_Modem()
{
    dut_communication_ = std::make_unique<AT_Serial_Communication>();
}

bool AT_Modem::start() {
    return dut_communication_->init(name_);
}

bool AT_Modem::stop() {
    dut_communication_->deinit();
    return true;
}

bool AT_Modem::restart() {
    //return dut_communication_->restart();
    std::string AT_COMMAND = "AT+CFUN=0";
    dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_FLIGHT_MODE, "OK");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return true;
}

bool AT_Modem::is_alive() {
    std::string AT_COMMAND = "AT";
    AT_Response resp;// = send_AT_COMMAND(AT_COMMAND);
    size_t counter = 5;
    do {
        resp = dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_AT, "OK");
    } while (!resp.is_OK && --counter);
    return resp.is_OK;
}

bool AT_Modem::connect_to_network() {
    if (!set_ue_to_full_functionality()) return false;
    // bool ret = false;
    // std::string AT_COMMAND = "AT+COPS=1,2,\"" + main_config_g.network_names.at(0) + "\"";
    // size_t counter = 5;
    // AT_Response resp;
    // for (size_t i = 0; i < counter; ++i) {
    //     resp = dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_CONNECT_TO_NETWORK, "OK");
    //     if (!resp.error) {
    //         ret = true;
    //         break;
    //     }
    // }
    std::this_thread::sleep_for(std::chrono::seconds(timers_config_g.AT_CONNECT_TO_NETWORK));
    return true;
}

bool AT_Modem::is_connected() {
    std::string AT_COMMAND = "AT+COPS?";
    //std::string restart_trigger = "COPS: 0";
    //AT_Response resp = send_AT_COMMAND(AT_COMMAND);
    AT_Response resp;
    int counter = 5;
    for (int i = 0; i < counter; i++) {
        resp = dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, 2, "OK");
        if (!resp.error) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (resp.timeout) {
        if (!is_alive()) throw std::runtime_error(MODEM_DEAD);
        return false;
    }
    for (const std::string& network_name : main_config_g.network_names) {
        if (resp.response.find(network_name) != std::string::npos) return true;
    }
    return false;
}

bool AT_Modem::disconnect_from_network() {
    return set_to_airplane_mode();
}

bool AT_Modem::set_to_airplane_mode() {
    std::string AT_COMMAND = "AT+CFUN=4";
    AT_Response resp;
    for (size_t i = 0; i < 3; i++) {
        size_t counter = 5;
        do {
            resp = dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_FLIGHT_MODE, "OK");
        } while (resp.error && --counter);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        resp = get_functionality();
        if (resp.response.find("+CFUN: 4") != std::string::npos) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return false;
}

AT_Response AT_Modem::get_functionality() {
    std::string AT_COMMAND = "AT+CFUN?";
    AT_Response resp;// = send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_SET_FULL_FUNCTIONALITY);
    size_t counter = 5;
    do {
        resp = dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_SET_FULL_FUNCTIONALITY, "OK");
    } while (resp.error && --counter);
    return resp;
}

bool AT_Modem::set_ue_to_full_functionality() {
    std::string AT_COMMAND = "AT+CFUN=1";
    AT_Response resp;// = send_AT_COMMAND(AT_COMMAND);
    for (size_t i = 0; i < 3; i++) {
        size_t counter = 5;
        do {
            resp = dut_communication_->send_AT_COMMAND_nonblocking(AT_COMMAND, timers_config_g.AT_FLIGHT_MODE, "OK");
        } while (resp.error && --counter);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        resp = get_functionality();
        if (resp.response.find("+CFUN: 1") != std::string::npos) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return false;
}