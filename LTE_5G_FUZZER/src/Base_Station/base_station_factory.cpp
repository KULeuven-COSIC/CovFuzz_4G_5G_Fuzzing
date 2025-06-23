#include "Base_Station/base_station_factory.h"

#include "Base_Station/srsGNB.h"
#include "Base_Station/srsGNB_ZMQ.h"
#include "Base_Station/srsENB.h"
#include "Base_Station/srsENB_ZMQ.h"

#include <stdexcept>

std::unique_ptr<Base_Station> Base_Station_Factory::get_base_station_by_protocol_name(PROTOCOL_STACK_NAME protocol_stack_name)
{
    switch(protocol_stack_name) {
        case PROTOCOL_STACK_NAME::SRSRAN_4G_SRSUE: return std::make_unique<SRSENB_ZMQ>();
        case PROTOCOL_STACK_NAME::SRSRAN_4G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_4G_DUMMY:
            return std::make_unique<SRSENB>();
        case PROTOCOL_STACK_NAME::SRSRAN_5G_SRSUE: return std::make_unique<SRSGNB_ZMQ>();
        case PROTOCOL_STACK_NAME::SRSRAN_5G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_DUMMY:
            return std::make_unique<SRSGNB>();
        default: break;
    }
    throw std::runtime_error("Unsupported protocol stack name");
}