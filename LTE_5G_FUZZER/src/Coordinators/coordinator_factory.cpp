#include "Coordinators/coordinator_factory.h"

#include "Coordinators/dummy_coordinator.h"
#include "Coordinators/base_coordinator.h"
#include "Coordinators/srs_coordinator.h"
#include "Coordinators/active_coordinator.h"

std::unique_ptr<Base_Coordinator> Coordinator_Factory::get_coordinator_by_protocol_name(PROTOCOL_STACK_NAME protocol_stack_name) {
    switch(protocol_stack_name) {
        case PROTOCOL_STACK_NAME::SRSRAN_4G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_COTS_UE:
            return std::make_unique<Active_Coordinator>();
        case PROTOCOL_STACK_NAME::SRSRAN_4G_SRSUE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_SRSUE:
            return std::make_unique<SRS_Coordinator>();
        case PROTOCOL_STACK_NAME::SRSRAN_4G_DUMMY:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_DUMMY:
            return std::make_unique<Dummy_Coordinator>();
        default: break;
    }
    // else if (t == TARGET::DIGI_XBEE3 || t == TARGET::TELIT || t == TARGET::QUALCOMM || t == TARGET::ZTEWELINK ||
    //         t == TARGET::SIERRA || t == TARGET::Samsung_Galaxy_A7 || t == TARGET::NOKIA_5G21_GATEWAY || t == TARGET::FIBOCOM || 
    //         t == TARGET::QUECTEL_RM500Q || t == TARGET::SIMCOM_5G) {
    //     return std::make_unique<Active_Coordinator>();
    // } else if (t == TARGET::DUMMY) {
    //     return std::make_unique<Dummy_Coordinator>();
    // } else if (t == TARGET::NRF_9160 || t == TARGET::ARDUINO) {
    //     return std::make_unique<Passive_Coordinator>();
    // }
    throw std::runtime_error("Cannot create coordinator: Unknown target");
}