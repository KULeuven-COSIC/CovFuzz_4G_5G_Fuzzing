#include "zmq_server.h"

#include <optional>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>

#include <zmq.hpp>

int main() {
    ZMQ_Server server(true);
    std::vector<uint8_t> v = {1, 3, 5};
    server.bind("tcp://*:5555");
    while (true)
    {
        boost::optional<zmq::message_t> msg = server.recv_non_blocking(5);
        if (msg) {
            std::cout << "Received by server: " << msg.value().to_string() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (server.send("Server: " + msg.value().to_string()))
                std::cout << "Sent successfully by server: " << msg.value().to_string() << std::endl;
            std::cout << std::endl;
        }
    }
    
    return 0;
}