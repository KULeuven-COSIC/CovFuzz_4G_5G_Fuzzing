#include "Communication/shm_RRC_communication.h"

#include "packet.h"

#include "shared_memory.h"

#include <cstddef> // defines uint8_t
#include <memory>

SHM_RRC_Communication::SHM_RRC_Communication() : SHM_Layer_Communication(static_cast<int>(EnumMutex::SHM_MUTEX_RRC)) {}

void SHM_RRC_Communication::send(Packet &packet) {
    int payload_idx = 2;
    uint32_t pdu_size = packet.get_size() + payload_idx;
    
    /* Prepare the packet */
    std::unique_ptr<uint8_t[]> msg = std::make_unique<uint8_t[]>(pdu_size);
    msg[0] = static_cast<uint8_t>(packet.get_channel());
    msg[1] = static_cast<uint8_t>(packet.get_direction());
    std::memcpy(&msg[payload_idx], &(packet.get_data()[0]), packet.get_size());

    /* Send the packet via shared memory */
    shm_->write_bytes(layer_, msg.get(), pdu_size);
}

Packet SHM_RRC_Communication::receive() {
    std::unique_ptr<uint8_t[]> msg_ptr = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);
    uint32_t msg_size;
    if (!shm_->read_bytes(layer_, msg_ptr.get(), msg_size)) {
        return {};
    }
    uint8_t cur_idx = 0;
    CHANNEL channel = static_cast<CHANNEL>(msg_ptr[cur_idx++]);
    uint8_t direction = static_cast<uint8_t>(msg_ptr[cur_idx++]);
    return Packet(reinterpret_cast<uint8_t*>(&msg_ptr[cur_idx]), msg_size - cur_idx, channel, direction, layer_);
}