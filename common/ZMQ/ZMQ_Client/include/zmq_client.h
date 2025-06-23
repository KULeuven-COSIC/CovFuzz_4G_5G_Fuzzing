#ifndef _ZMQ_Client_
#define _ZMQ_Client_

#include <zmq.hpp>
#include <string>
#include <boost/optional.hpp>

class ZMQ_Client {
public:
    ZMQ_Client(bool verbose = false);
    ~ZMQ_Client();
    bool connect(const std::string& server_address);
    void disconnect();
    boost::optional<zmq::message_t> recv();
    boost::optional<zmq::message_t> recv_non_blocking(size_t timer = 30);
    bool send(zmq::message_t&& msg);
    bool send(const std::string& msg);
    bool send(void* msg, size_t size);

    // Helpers
    std::string send_recv(const std::string& msg); // returns empty string if smth went wrong
    std::string send_recv_non_blocking(const std::string& msg, size_t timer = 30); // returns empty string if smth went wrong


private:
    zmq::context_t context;
    zmq::socket_t socket;
    std::string server_address;
    bool verbose;
};

#endif //_ZMQ_Client_