#include "Core_Network/core_network_factory.h"

#include "Core_Network/srsEPC.h"
#include "Core_Network/Open5Gs.h"

#include <stdexcept>

std::unique_ptr<Core_Network> Core_Network_Factory::get_core_network_by_protocol_name(PROTOCOL_STACK_NAME protocol_stack_name)
{
    switch(protocol_stack_name) {
        case PROTOCOL_STACK_NAME::SRSRAN_4G_SRSUE:
        case PROTOCOL_STACK_NAME::SRSRAN_4G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_4G_DUMMY:
            return std::make_unique<srsEPC>();
        case PROTOCOL_STACK_NAME::SRSRAN_5G_DUMMY:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_COTS_UE:
            return std::make_unique<Open5Gs>();
        default: break;
    }
    throw std::runtime_error("Unsupported protocol stack name");
}