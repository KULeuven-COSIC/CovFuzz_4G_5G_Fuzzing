#include "zmq_server.h"

#include <iostream>
#include <boost/optional.hpp>

#include <thread>
#include <chrono>

ZMQ_Server::ZMQ_Server(bool verbose) {
    socket = zmq::socket_t(context, zmq::socket_type::rep);
    socket.set(zmq::sockopt::linger, 0);
    this->verbose = verbose;
}

ZMQ_Server::~ZMQ_Server(){
    socket.close();
    if (verbose) std::cout << "Socket is closed\n";
}

bool ZMQ_Server::bind(const std::string& bind_address) {
    try {
        socket.bind(bind_address);
        if (verbose) std::cout << "Successfully bound to this address: " << bind_address << "\n";
        return true;
    } catch(const zmq::error_t& ex) {
        if (verbose) {
            std::cout << "Failed to bind to this address: " << bind_address << "\n";
            std::cout << "Exception: " << ex.what() << std::endl;
        }
        return false;
    }
}

void ZMQ_Server::unbind(const std::string& bind_address) {
    try {
        socket.unbind(bind_address);
    } catch(const zmq::error_t& ex) {
        if (verbose) {
            std::cout << "Failed to unbind from this address: " << bind_address << "\n";
            std::cout << "Exception: " << ex.what() << std::endl;
        }
    }
}

boost::optional<zmq::message_t> ZMQ_Server::recv(){
    zmq::message_t message;
    try {
        if (verbose) std::cout << "Trying to receive a message" << std::endl;
        zmq::recv_result_t res = socket.recv(message, zmq::recv_flags::none);
        if (res) return message;
        throw std::runtime_error("Failed to receive a message");
    } catch (const zmq::error_t& ex) {
        if (verbose) std::cout << "ZMQ exception during recv: " << ex.what() << "\n";
    } catch (...) {
        if (verbose) std::cout << "Failed to receive a message\n";
    }
    return {};
}

boost::optional<zmq::message_t> ZMQ_Server::recv_non_blocking(size_t timer) {
    timer += 2;
    size_t sleep_in_ms = 10;
    size_t new_timer = timer * (1000.0 / sleep_in_ms);
    zmq::message_t message;
    try {
        while (--new_timer) {
            zmq::recv_result_t res = socket.recv(message, zmq::recv_flags::dontwait);
            if (res) return message;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_in_ms));
        }
        throw std::runtime_error("");
    } catch (const zmq::error_t& ex) {
        if (verbose) std::cout << "ZMQ exception during recv: " << ex.what() << std::endl;
    } catch (...) {
        if (verbose) std::cout << "Failed to receive a message" << std::endl;
    }
    return {};
}

bool ZMQ_Server::send(zmq::message_t&& msg) {
    try {
        zmq::recv_result_t res = socket.send(msg, zmq::send_flags::none);
        if (res) return true;
        throw std::runtime_error("Failed to send a message");
    } catch (const zmq::error_t& ex) {
        if (verbose) std::cout << "ZMQ exception during send: " << ex.what() << "\n";
    } catch (...) {
        if (verbose) std::cout << "Failed to send a message\n";
    }
    return false;
}

bool ZMQ_Server::send(const std::string& msg) {
    return this->send(zmq::message_t(msg));
}

bool ZMQ_Server::send(void* msg, size_t size) {
    return this->send(zmq::message_t(msg, size));
}
