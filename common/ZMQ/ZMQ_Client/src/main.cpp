#include "zmq_client.h"

#include <boost/optional.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include <zmq.hpp>

int main() {
    ZMQ_Client client(true);
    client.connect("tcp://localhost:5555");
    int counter = 0;
    while (++counter)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!client.send(std::to_string(counter))) {
            std::cout << "Failed to send" << std::endl;
            continue;
        }
        std::cout << "sent counter: " << counter << std::endl;
        boost::optional<zmq::message_t> msg = client.recv_non_blocking(8);
        if (msg){
            std::cout << "Received: " << msg.value().to_string() << std::endl;
        }
    }
    

    return 0;
}