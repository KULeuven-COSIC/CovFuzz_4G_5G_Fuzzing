#include "UE/srsUE_factory.h"

#include "UE/srsUE_ZMQ.h"
#include "UE/srsUE.h"

std::unique_ptr<SRSUE_Base> SRSUE_Factory::get_ue_by_protocol_name(PROTOCOL_STACK_NAME protocol_stack_name)
{
    switch(protocol_stack_name) {
        case PROTOCOL_STACK_NAME::SRSRAN_4G_SRSUE: return std::make_unique<SRSUE_ZMQ>();
        default: break;
    }
    throw std::runtime_error("Unsupported protocol stack name");
}