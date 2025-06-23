#pragma once

#include "Protocol_Stack/Protocol_Stack.h"

#include "UE/srsUE_base.h"

class Protocol_Stack_with_SRSUE final : public Protocol_Stack {
public:
    Protocol_Stack_with_SRSUE();
    bool start();
    bool stop();
    bool restart();
    bool is_running();

private:
    std::unique_ptr<SRSUE_Base> srsue_;
};