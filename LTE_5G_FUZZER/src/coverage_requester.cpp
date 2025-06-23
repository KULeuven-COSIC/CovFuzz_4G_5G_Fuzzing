#include "coverage_requester.h"

#include "my_logger.h"

#include <boost/optional.hpp>
#include <string>

extern My_Logger my_logger_g;

bool Coverage_Requester::init(const std::string& server_address) {
    this->server_address = server_address;
    return zmq_client.connect(server_address);
}

bool Coverage_Requester::get_code_coverage_map(size_t map_size, uint8_t* map_ptr) {
    if (!zmq_client.send(get_coverage_map_cmd)) {
        my_logger_g.logger->error("Failed to send command {} to DUT", get_coverage_map_cmd);
        throw std::runtime_error("Failed to send get_coverage_map_cmd command to DUT");
    }
    boost::optional<zmq::message_t> msg = zmq_client.recv_non_blocking(3);
    if (msg) {
        memcpy(map_ptr, msg.value().data(), map_size);
        return true;
    }
    return false;
}

uint32_t Coverage_Requester::get_code_coverage_map_size() {
    if (!zmq_client.send(get_coverage_map_size_cmd)) {
        my_logger_g.logger->error("Failed to send command {} to DUT", get_coverage_map_size_cmd);
        throw std::runtime_error("Failed to send get_coverage_map_size_cmd command to DUT");
    }
    my_logger_g.logger->debug("Sent command {} to DUT", get_coverage_map_size_cmd);
    boost::optional<zmq::message_t> msg = zmq_client.recv_non_blocking(3);
    if (msg) {
        std::string response = msg.value().to_string();
        my_logger_g.logger->debug("Received response: {}", response);
        return std::stoul(response);
    }
    return 0;
}


bool Coverage_Requester::reset_code_coverage_map() {
    if (!zmq_client.send(reset_coverage_map_cmd)) {
        my_logger_g.logger->error("Failed to send command {} to DUT", reset_coverage_map_cmd);
        throw std::runtime_error("Failed to send reset_coverage_map_cmd command to DUT");
    }
    my_logger_g.logger->debug("Sent command {} to DUT", reset_coverage_map_cmd);
    boost::optional<zmq::message_t> msg = zmq_client.recv_non_blocking(3);
    if (msg) {
        std::string response = msg.value().to_string();
        my_logger_g.logger->debug("Received response: {}", response);
        if (response == "OK") return true;
    }
    return false;
}

void Coverage_Requester::end_communication() {
    if (!zmq_client.send(end_cmd)) {
        my_logger_g.logger->error("Failed to send command {} to DUT", end_cmd);
        throw std::runtime_error("Failed to send end_cmd command to DUT");
    }
    my_logger_g.logger->debug("Sent command {} to DUT", end_cmd);
    boost::optional<zmq::message_t> msg = zmq_client.recv_non_blocking(3);
    if (msg) {
        std::string response = msg.value().to_string();
        my_logger_g.logger->debug("Received response: {}", response);
    }
}