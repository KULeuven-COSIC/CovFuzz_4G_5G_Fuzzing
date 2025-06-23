#include "UE/UE_factory.h"

#include "UE/AT_Modem.h"
#include "UE/Android_UE.h"
#include "UE/Dummy_UE.h"

#include <stdexcept>

std::unique_ptr<UE> UE_Factory::get_ue_by_protocol_name(UE_Name ue_name)
{
    switch(ue_name) {
        case UE_Name::AT_Modem: return std::make_unique<AT_Modem>();
        case UE_Name::Android_UE: return std::make_unique<Android_UE>();
        case UE_Name::DUMMY_UE: return std::make_unique<Dummy_UE>();
        default: break;
    }
    throw std::runtime_error("Unknown UE name");
}