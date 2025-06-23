#include "zmq_client.h"

#include <iostream>
#include <boost/optional.hpp>
#include <thread>
#include <chrono>
#include <string>

ZMQ_Client::ZMQ_Client(bool verbose) {
    socket = zmq::socket_t(context, zmq::socket_type::req);
    socket.set(zmq::sockopt::linger, 0);
    socket.set(zmq::sockopt::req_relaxed, 1);
    socket.set(zmq::sockopt::req_correlate, 1);
    this->verbose = verbose;
}

ZMQ_Client::~ZMQ_Client(){
    if(verbose) std::cout << "Disconnecting the socket..." << std::endl;
    socket.disconnect(server_address);
    if(verbose) std::cout << "Closing the socket..." << std::endl;
    socket.close();
    if(verbose) std::cout << "Client socket is closed\n";
}

bool ZMQ_Client::connect(const std::string& server_address){
    try {
        socket.connect(server_address);
        this->server_address = server_address;
        if (verbose) std::cout << "Successfully connected to this address: " << server_address << "\n";
        return true;
    } catch(...) {
        if (verbose) std::cout << "Failed to connect to this address: " << server_address << "\n";
        return false;
    }
}

void ZMQ_Client::disconnect() {
    socket.disconnect(server_address);
    server_address = "";
}

boost::optional<zmq::message_t> ZMQ_Client::recv() {
    zmq::message_t message;
    try {
        zmq::recv_result_t res = socket.recv(message, zmq::recv_flags::none);
        if (res) return message;
        throw std::runtime_error("");
    } catch (const zmq::error_t& ex) {
        if (verbose) std::cout << "ZMQ exception during recv: " << ex.what() << "\n";
    } catch (...) {
        if (verbose) std::cout << "Failed to receive a message\n";
    }
    return {};
}

boost::optional<zmq::message_t> ZMQ_Client::recv_non_blocking(size_t timer) {
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

bool ZMQ_Client::send(zmq::message_t&& msg) {
    try {
        //if (!connect(server_address)) throw std::runtime_error("Could not connect to address " + server_address);
        zmq::recv_result_t res = socket.send(msg, zmq::send_flags::none);
        if (res) return true;
        throw std::runtime_error("");
    } catch (const zmq::error_t& ex) {
        if (verbose) std::cout << "Client ZMQ exception during send: " << ex.what() << "\n";
    } catch (...) {
        if (verbose) std::cout << "Client failed to send a message" << std::endl;
    }
    return false;
}

bool ZMQ_Client::send(const std::string& msg) {
    return this->send(zmq::message_t(msg));
}


bool ZMQ_Client::send(void* msg, size_t size) {
    return this->send(zmq::message_t(msg, size));
}

std::string ZMQ_Client::send_recv(const std::string& msg){
    if (this->send(msg)){
        boost::optional<zmq::message_t> msg = this->recv();
        if (msg) {
            return msg.value().to_string();
        }
    }
    return {};
}

std::string ZMQ_Client::send_recv_non_blocking(const std::string& msg, size_t timer){
    if (this->send(msg)){
        boost::optional<zmq::message_t> msg = this->recv_non_blocking(timer);
        if (msg) {
            return msg.value().to_string();
        }
    }
    return {};
}