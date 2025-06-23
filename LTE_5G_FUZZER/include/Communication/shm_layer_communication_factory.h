#pragma once

#include "Communication/shm_layer_communication.h"

#include <memory>

class SHM_Layer_Communication_Factory {
public:
    static std::unique_ptr<SHM_Layer_Communication> get_shm_layer_communication_instance_by_layer_num(int layer_num);
};