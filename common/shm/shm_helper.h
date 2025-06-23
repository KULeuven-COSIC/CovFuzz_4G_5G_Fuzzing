#pragma once

#include "shared_memory.h"
#include "common_utils.h"

#include <memory>
#include <cstddef>

class SHM_Helper {

public:
    SHM_Helper(const std::string name, bool is_server = false, size_t buffer_size_per_layer = MSG_MAX_SIZE);
    void send_rrc_msg(uint8_t direction, enum CHANNEL channel_name, uint8_t* payload, uint32_t& payload_size);
    void send_mac_msg(uint8_t event, uint8_t direction, uint16_t rnti_type, uint16_t rnti_number, uint16_t frame_number, uint16_t subframe_number, uint8_t *payload, uint32_t &payload_size, int8_t preamble);
    void send_mac_msg_nr(uint8_t event, uint8_t direction, uint16_t rnti_type, uint16_t rnti_number, uint16_t frame_number, uint16_t subframe_number, uint8_t *payload, uint32_t &payload_size, int8_t preamble);

private:
    const std::string name;
    std::unique_ptr<SHM> shm;

};