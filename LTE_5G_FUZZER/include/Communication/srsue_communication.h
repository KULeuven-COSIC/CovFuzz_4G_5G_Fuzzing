#pragma once

#include "ZMQ/ZMQ_Client/include/zmq_client.h"
#include "ZMQ/ZMQ_Server/include/zmq_server.h"

#include <string>

class srsue_communication {
public:
    srsue_communication() {}
    
    /* ZMQ Communication with refUE */
    bool init_zmq_communication();
    void deinit_zmq_communication();
    bool reset_stack();
    bool connect_to_network();
    bool disconnect_from_network();
    bool is_connected();
    bool is_disconnected();
    bool shut_down();
    std::string receive_msg();

private:
    ZMQ_Client zmq_client = ZMQ_Client(true);
    std::string server_address = "tcp://127.0.0.1:5556";

    ZMQ_Server zmq_server = ZMQ_Server(true);
    std::string bind_address = "tcp://*:5557";

};
