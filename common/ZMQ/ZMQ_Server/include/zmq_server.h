#ifndef _ZMQ_SERVER_
#define _ZMQ_SERVER_

#include <zmq.hpp>
#include <string>
#include <boost/optional.hpp>

class ZMQ_Server {
public:
    ZMQ_Server(bool verbose = false);
    ~ZMQ_Server();
    bool bind(const std::string& bind_address);
    void unbind(const std::string& bind_address);
    boost::optional<zmq::message_t> recv();
    boost::optional<zmq::message_t> recv_non_blocking(size_t timer = 30);
    bool send(zmq::message_t&& msg);
    bool send(const std::string& msg);
    bool send(void* msg, size_t size);


private:
    zmq::context_t context;
    zmq::socket_t socket;
    bool verbose;
};

#endif //_ZMQ_SERVER_

