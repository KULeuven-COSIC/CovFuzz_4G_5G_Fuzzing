#include "Protocol_Stack/protocol_stack_factory.h"

#include "Protocol_Stack/Protocol_Stack_Base.h"
#include "Protocol_Stack/Dummy_stack.h"
#include "Protocol_Stack/Protocol_Stack.h"
#include "Protocol_Stack/Protocol_Stack_with_srsUE.h"

std::unique_ptr<Protocol_Stack_Base> Protocol_Stack_Factory::get_protocol_stack_by_protocol_name(PROTOCOL_STACK_NAME protocol_stack_name)
{
    switch(protocol_stack_name) {
        case PROTOCOL_STACK_NAME::SRSRAN_4G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_4G_DUMMY:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_DUMMY:
            return std::make_unique<Protocol_Stack>(); 
        case PROTOCOL_STACK_NAME::SRSRAN_4G_SRSUE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_SRSUE:
            return std::make_unique<Protocol_Stack_with_SRSUE>();
        case PROTOCOL_STACK_NAME::DUMMY:
            return std::make_unique<Dummy_stack>();
    }
    throw std::runtime_error("Unsupported protocol stack name");
}