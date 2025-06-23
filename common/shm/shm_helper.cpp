#include "shm_helper.h"
#include <iostream>

SHM_Helper::SHM_Helper(const std::string name, bool is_server, size_t buffer_size_per_layer) : name(name) {
    std::cout << "SHM Helper Constructor START" << std::endl;
    shm = std::make_unique<SHM>(is_server, name, buffer_size_per_layer);
    std::cout << "SHM Helper Constructor END" << std::endl;
}

// TODO: change the direction & pdu_type type
void SHM_Helper::send_rrc_msg(uint8_t direction, enum CHANNEL channel_name, uint8_t* payload, uint32_t& payload_size) {
    if (payload == NULL || payload_size == 0) return;
    int layer = static_cast<int>(EnumMutex::SHM_MUTEX_RRC);

#if DEBUG_MODE
    std::cout << "Old payload: ";
    print_bytes(payload, payload_size);
#endif

    size_t payload_offset = 2;
    uint32_t bytes_to_send_size = payload_size + payload_offset;
    std::unique_ptr<uint8_t[]> byte_buffer = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);

    byte_buffer[0] = static_cast<uint8_t>(channel_name);
    byte_buffer[1] = static_cast<uint8_t>(direction);
    std::copy(payload, payload + payload_size, byte_buffer.get() + payload_offset);
    shm->write_bytes(layer, byte_buffer.get(), bytes_to_send_size);

    uint32_t bytes_to_read_size;
    if (!shm->read_bytes(layer, byte_buffer.get(), bytes_to_read_size)) {
        return;
    }
    std::copy(byte_buffer.get() + payload_offset, byte_buffer.get() + payload_offset + bytes_to_read_size, payload);
    payload_size = bytes_to_read_size - payload_offset;

#if DEBUG_MODE
    std::cout << "New payload: ";
    print_bytes(payload, payload_size);
#endif
}

void SHM_Helper::send_mac_msg(
                    uint8_t event,
                    uint8_t direction,
                    uint16_t rnti_type,
                    uint16_t rnti_number,
                    uint16_t frame_number,
                    uint16_t subframe_number,
                    uint8_t *payload, 
                    uint32_t &payload_size,
                    int8_t preamble)
{
    if (payload == NULL || payload_size == 0) return;
    int layer = static_cast<int>(EnumMutex::SHM_MUTEX_MAC);

#if DEBUG_MODE
    std::cout << "Old payload: ";
    print_bytes(payload, payload_size);
#endif

    std::unique_ptr<uint8_t[]> byte_buffer = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);

    size_t payload_offset, s_idx = 0;

    byte_buffer[s_idx++] = event;
    // Wireshark payload starts here
    byte_buffer[s_idx++] = FDD_RADIO;
    byte_buffer[s_idx++] = direction;
    byte_buffer[s_idx++] = rnti_type;
    if (rnti_type == WS_C_RNTI || rnti_type == WS_RA_RNTI)
    {
        byte_buffer[s_idx++] = MAC_LTE_RNTI_TAG;
        byte_buffer[s_idx++] = (rnti_number >> 8) & 0xFF;
        byte_buffer[s_idx++] = rnti_number & 0xFF;
    }
    int fsf = (frame_number << 4) + subframe_number;

    byte_buffer[s_idx++] = MAC_LTE_FRAME_SUBFRAME_TAG;
    byte_buffer[s_idx++] = (fsf >> 8) & 0xFF;
    byte_buffer[s_idx++] = fsf & 0xFF;

    if (preamble != -1)
    {
        byte_buffer[s_idx++] = MAC_LTE_SEND_PREAMBLE_TAG;
        byte_buffer[s_idx++] = preamble;
        byte_buffer[s_idx++] = 0; /* rach attempt - always 0 for us (not sure of this) */
    }

    byte_buffer[s_idx++] = MAC_LTE_PAYLOAD_TAG;
    payload_offset = s_idx;
    
    uint32_t bytes_to_send_size = payload_size + payload_offset;
    std::copy(payload, payload + payload_size, byte_buffer.get() + payload_offset);
    shm->write_bytes(layer, byte_buffer.get(), bytes_to_send_size);

    uint32_t bytes_to_read_size;
    if (!shm->read_bytes(layer, byte_buffer.get(), bytes_to_read_size)) {
        return;
    }
    std::copy(byte_buffer.get() + payload_offset, byte_buffer.get() + payload_offset + bytes_to_read_size, payload);
    payload_size = bytes_to_read_size - payload_offset;

#if DEBUG_MODE
    std::cout << "New payload: ";
    print_bytes(payload, payload_size);
#endif
}


void SHM_Helper::send_mac_msg_nr(
                    uint8_t event,
                    uint8_t direction,
                    uint16_t rnti_type,
                    uint16_t rnti_number,
                    uint16_t frame_number,
                    uint16_t subframe_number,
                    uint8_t *payload, 
                    uint32_t &payload_size,
                    int8_t preamble)
{
    if (payload == NULL || payload_size == 0) return;
    int layer = static_cast<int>(EnumMutex::SHM_MUTEX_MAC);

#if DEBUG_MODE
    std::cout << "Old payload: ";
    print_bytes(payload, payload_size);
#endif

    std::unique_ptr<uint8_t[]> byte_buffer = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);

    size_t payload_offset, s_idx = 0;

    byte_buffer[s_idx++] = event;
    // Wireshark payload starts here
    byte_buffer[s_idx++] = NR_TDD_RADIO;
    byte_buffer[s_idx++] = direction;
    byte_buffer[s_idx++] = rnti_type;
    if (rnti_type == NR_C_RNTI || rnti_type == NR_RA_RNTI)
    {
        byte_buffer[s_idx++] = MAC_NR_RNTI_TAG;
        byte_buffer[s_idx++] = (rnti_number >> 8) & 0xFF;
        byte_buffer[s_idx++] = rnti_number & 0xFF;
    }

    byte_buffer[s_idx++] = MAC_NR_FRAME_SLOT_TAG;
    byte_buffer[s_idx++] = (frame_number>>8) & 0xFF;
    byte_buffer[s_idx++] = frame_number & 0xFF;
    byte_buffer[s_idx++] = (subframe_number>>8) & 0xFF;
    byte_buffer[s_idx++] = subframe_number & 0xFF;

    byte_buffer[s_idx++] = MAC_NR_PAYLOAD_TAG;
    payload_offset = s_idx;
    
    uint32_t bytes_to_send_size = payload_size + payload_offset;
    std::copy(payload, payload + payload_size, byte_buffer.get() + payload_offset);
    shm->write_bytes(layer, byte_buffer.get(), bytes_to_send_size);

    uint32_t bytes_to_read_size;
    if (!shm->read_bytes(layer, byte_buffer.get(), bytes_to_read_size)) {
        return;
    }
    std::copy(byte_buffer.get() + payload_offset, byte_buffer.get() + payload_offset + bytes_to_read_size, payload);
    payload_size = bytes_to_read_size - payload_offset;

#if DEBUG_MODE
    std::cout << "New payload: ";
    print_bytes(payload, payload_size);
#endif
}

SHM_Helper shm_helper_g(SHM_NAME);