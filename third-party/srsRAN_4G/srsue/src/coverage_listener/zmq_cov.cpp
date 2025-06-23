#include "instrumentation.h"
#include "zmq_cov.h"

#include <iostream>
#include <zmq.hpp>
#include <string>

extern feedback_t feedback;

namespace zmq_cov {

    // Private part
    static ZMQ_Server zmq_server;

    static std::string recv_message() {
        boost::optional<zmq::message_t> msg = zmq_server.recv();
        if (msg) {
            return msg.value().to_string();
        }
        return {};
    }

    static bool send_bitmap_data() {
        return zmq_server.send(feedback.guard_map, feedback.guard_count);
    }

    static void reset_bitmap() {
        memset(feedback.guard_map, 0, feedback.guard_count);
    }

    // Public function
    void worker_loop() {
        zmq_server.bind("tcp://*:5566");
        std::string msg;
        while (msg != "OVER") {
            msg = recv_message();
            if (msg == "GET_COVERAGE_MAP") {
                if (send_bitmap_data()) {
                    printf("Sent bitmap data successfully!\n");
                } else {
                    printf("Sending bitmap data failed!\n");
                }
            } else if (msg == "RESET_COVERAGE_MAP") {
                reset_bitmap();
                zmq_server.send("OK");
            } else if (msg == "GET_COVERAGE_MAP_SIZE") {
                printf("Sending size: %lu\n", feedback.guard_count);
                zmq_server.send(std::to_string(feedback.guard_count));
            }
        }
    }

}