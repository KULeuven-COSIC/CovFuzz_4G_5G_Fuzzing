#include "Protocol_Stack/Protocol_type.h"

#include "common_utils.h"

#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::string> get_field_prefixes_by_layer_idx(uint8_t mutex_name, PROTOCOL_TYPE protocol_type) {
  if (protocol_type == PROTOCOL_TYPE::PROTO_4G) {
    if (mutex_name == SHM_MUTEX_MAC) return {"mac-lte", "rlc-lte", "pdcp-lte"};
    else if (mutex_name == SHM_MUTEX_RRC) return {""};// Allow mutation of both rrc and nas fields on rrc layer
  } else if (protocol_type == PROTOCOL_TYPE::PROTO_5G) {
    if (mutex_name == SHM_MUTEX_MAC) return {"mac-nr", "rlc-nr", "pdcp-nr"};
    else if (mutex_name == SHM_MUTEX_RRC) return {""};
  } else {
    throw std::runtime_error("(Get field prefix by layer idx) Unsupported protocol " + std::to_string(int(protocol_type)));   
  }
  throw std::runtime_error("(Get field prefix by layer idx) Invalid mutex value " + std::to_string(mutex_name));
}

PROTOCOL_TYPE get_protocol_type_by_protocol_stack_name(PROTOCOL_STACK_NAME protocol_stack_name)
{
    switch(protocol_stack_name) {
        case PROTOCOL_STACK_NAME::SRSRAN_4G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_4G_SRSUE:
        case PROTOCOL_STACK_NAME::SRSRAN_4G_DUMMY:
            return PROTOCOL_TYPE::PROTO_4G;
        case PROTOCOL_STACK_NAME::SRSRAN_5G_COTS_UE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_SRSUE:
        case PROTOCOL_STACK_NAME::SRSRAN_5G_DUMMY:
            return PROTOCOL_TYPE::PROTO_5G;
        case PROTOCOL_STACK_NAME::DUMMY:
            return PROTOCOL_TYPE::DUMMY;
    }
    throw std::runtime_error("Unsupported protocol stack name");
}