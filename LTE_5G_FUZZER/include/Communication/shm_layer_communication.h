#pragma once

#include "packet.h"

#include "shared_memory.h"

#include <atomic>
#include <memory>

class SHM_Layer_Communication {
public:
    SHM_Layer_Communication(int layer);
    virtual ~SHM_Layer_Communication() {}
    virtual void send(Packet& packet) = 0;
    virtual Packet receive() = 0;

    static volatile int& is_active;

protected:
    const int layer_;
    std::unique_ptr<SHM> shm_;
};