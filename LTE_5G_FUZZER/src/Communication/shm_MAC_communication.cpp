#include "Communication/shm_MAC_communication.h"

#include "packet.h"

#include "shared_memory.h"

#include <memory>

SHM_MAC_Communication::SHM_MAC_Communication() : SHM_Layer_Communication(static_cast<int>(EnumMutex::SHM_MUTEX_MAC)) {}

void SHM_MAC_Communication::send(Packet &packet) {
    int payload_idx = 1;
    uint32_t pdu_size = packet.get_size() + payload_idx;
    
    /* Prepare the packet */
    std::unique_ptr<uint8_t[]> msg = std::make_unique<uint8_t[]>(pdu_size);
    msg[0] = packet.get_event();
    std::memcpy(&msg[payload_idx], &(packet.get_data()[0]), packet.get_size());

    /* Send the packet via shared memory */
    shm_->write_bytes(layer_, msg.get(), pdu_size);
}

Packet SHM_MAC_Communication::receive() {
    std::unique_ptr<uint8_t[]> msg_ptr = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);
    uint32_t msg_size;
    if (!shm_->read_bytes(layer_, msg_ptr.get(), msg_size)) {
        return {};
    }

    uint8_t event = msg_ptr[0];
    uint8_t direction = msg_ptr[2];

    uint8_t cut_idx = 1; /* Event is not needed for dissection */

    
    return Packet(&msg_ptr[cut_idx], msg_size - cut_idx, event, direction, layer_);
}