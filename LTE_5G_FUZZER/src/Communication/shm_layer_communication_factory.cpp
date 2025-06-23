#include "Communication/shm_layer_communication_factory.h"

#include "Communication/shm_RRC_communication.h"
#include "Communication/shm_MAC_communication.h"

#include "common_utils.h"
#include "shared_memory.h"

#include <memory>
#include <stdexcept>

std::unique_ptr<SHM_Layer_Communication> SHM_Layer_Communication_Factory::get_shm_layer_communication_instance_by_layer_num(int layer_num) {
    if (layer_num == static_cast<int>(EnumMutex::SHM_MUTEX_RRC)) {
        return std::make_unique<SHM_RRC_Communication>();
    } else if (layer_num == static_cast<int>(EnumMutex::SHM_MUTEX_MAC)) {
        return std::make_unique<SHM_MAC_Communication>();
    }
    throw std::runtime_error("Failed to get the shm layer instance");
}