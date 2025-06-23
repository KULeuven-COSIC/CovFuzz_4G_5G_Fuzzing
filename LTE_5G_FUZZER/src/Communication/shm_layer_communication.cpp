#include "Communication/shm_layer_communication.h"

#include "shared_memory.h"

volatile int& SHM_Layer_Communication::is_active = SHM::keep_running;
SHM_Layer_Communication::SHM_Layer_Communication(int layer) : layer_(layer) {
    shm_ = std::make_unique<SHM>(SHM_SERVER, SHM_NAME, MSG_MAX_SIZE);
}