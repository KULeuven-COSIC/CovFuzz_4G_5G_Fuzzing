#include "Communication/srsue_communication.h"

#include "ZMQ/ZMQ_Client/include/zmq_client.h"
#include "ZMQ/ZMQ_Server/include/zmq_server.h"

#include "Configs/Fuzzing_Settings/timers_config.h"
#include "my_logger.h"

#include <boost/optional.hpp>

#include <string>

extern Timers_Config timers_config_g;
extern My_Logger my_logger_g;

/* ZMQ Communication with refUE */
bool srsue_communication::init_zmq_communication() {
    if (!zmq_server.bind(bind_address) || !zmq_client.connect(server_address)) {
        my_logger_g.logger->error("Initing ZMQ communication failed\n");
        return false;
    }
    return true;
}

void srsue_communication::deinit_zmq_communication() {
    zmq_server.unbind(bind_address);
    zmq_client.disconnect();
}

bool srsue_communication::reset_stack() {
    std::string reset_cmd = "reset";
    my_logger_g.logger->debug("Sending {}", reset_cmd);
    if (zmq_client.send_recv_non_blocking(reset_cmd, 5).empty()) return false;
    my_logger_g.logger->debug("Received response for {} command", reset_cmd);
    return true;
}

bool srsue_communication::connect_to_network() {
    std::string connect_cmd = "connect";
    my_logger_g.logger->debug("Sending {}", connect_cmd);
    if (zmq_client.send_recv_non_blocking(connect_cmd, 5).empty()) return false;
    my_logger_g.logger->debug("Received response for {} command", connect_cmd);
    return true;
}

bool srsue_communication::disconnect_from_network() {
    std::string disconnect_cmd = "disconnect";
    my_logger_g.logger->debug("Sending {}", disconnect_cmd);
    if (zmq_client.send_recv_non_blocking(disconnect_cmd, 3).empty()) return false;
    my_logger_g.logger->debug("Received response for {} command", disconnect_cmd);
    return true;
}

bool srsue_communication::is_connected() {
    std::string is_connected_cmd = "connected?";
    if (zmq_client.send_recv_non_blocking(is_connected_cmd, 3).empty()) return false;
    return true;
}

bool srsue_communication::is_disconnected() {
    std::string is_disconnected_cmd = "disconnected?";
    if (zmq_client.send_recv_non_blocking(is_disconnected_cmd, 3).empty()) return false;
    return true;
}

bool srsue_communication::shut_down() {
    std::string stop_cmd = "stop";
    if (zmq_client.send_recv_non_blocking(stop_cmd, 10).empty()) return false;
    return true;
}

std::string srsue_communication::receive_msg() {
    std::string response{};
    boost::optional<zmq::message_t> msg = zmq_server.recv_non_blocking(timers_config_g.AT_CONNECT_TO_NETWORK);
    if (msg) {
        response = msg.value().to_string();
        my_logger_g.logger->debug("Received response: {}", response);
        if (!zmq_server.send("OK")) {
            my_logger_g.logger->error("Failed to send OK to UE");
        }
    }
    return response;
}

