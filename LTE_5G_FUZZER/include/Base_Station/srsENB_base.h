#pragma once

#include "Base_Station/base_station.h"

#include <string>

/* Base class that implements the common functionalities for SRSENB and SRSENB_ZMQ */
class SRSENB_Base : public Base_Station {
public:
    virtual bool start() = 0;
    virtual bool stop();
    virtual bool restart();
    virtual bool is_alive();

    virtual const std::string& get_name() const = 0;
};