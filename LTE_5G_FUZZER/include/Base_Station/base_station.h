#pragma once

/* Abstract class representing a base-station */
class Base_Station {
public:
    virtual ~Base_Station() {};
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool restart() = 0;
    virtual bool is_alive() = 0;
};