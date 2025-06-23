#pragma once

#include <string>

class SRSUE_Base {
public:
    virtual ~SRSUE_Base() = default;
    virtual bool start() = 0;
    virtual bool stop();
    virtual bool restart();
    virtual bool is_alive();

    virtual const std::string& get_name() const = 0;
};