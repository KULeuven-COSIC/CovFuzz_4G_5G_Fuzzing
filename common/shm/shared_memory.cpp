#include "shared_memory.h"

#include <atomic>
#include <iomanip>
#include <iostream>
#include <memory>

#define DEBUG_MODE 0

volatile int SHM::keep_running = 1;

size_t get_random_bytes(uint8_t* arr, size_t max_size) {
    size_t rand_size = 4 + (rand() % (max_size - 4));
    std::cout << "Allocating " << rand_size << " bytes" << std::endl;
    for (size_t i = 0; i < rand_size; i++) {
        arr[i] = (uint8_t)(rand() % 256);
    }
    return rand_size;
}

void print_bytes(const std::unique_ptr<uint8_t[]>& arr, size_t size) {
    std::cout << "[ ";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::hex << (int)(arr[i]) << " ";
    }
    std::cout << std::dec << "]" << std::endl;
}

void print_bytes(const uint8_t* arr, size_t size) {
    std::cout << "[ ";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)(arr[i]) << " ";
    }
    std::cout << std::dec << "]" << std::endl;
}

SHM::SHM(bool is_server, const std::string name, size_t buffer_size_per_layer, size_t sync_var_size_per_layer, size_t layers_num) 
        : is_server(is_server), binary_wait_cond(is_server ? true : false), name(name), buffer_size_per_layer(buffer_size_per_layer), sync_var_size_per_layer(sync_var_size_per_layer), layers_num(layers_num) {

    shm_obj = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name.c_str(), boost::interprocess::read_write); // Create SHM object
    total_size_per_layer = buffer_size_per_layer + sync_var_size_per_layer;
    std::cout << "SHM object created!\n" << std::endl;

    if (is_server) shm_obj.truncate(total_size_per_layer * layers_num); // Allocate memory in SHM object

    for (size_t i = 0; i < layers_num; i++) {
        mapped_regions.push_back(std::make_pair(
            boost::interprocess::mapped_region(shm_obj, boost::interprocess::read_write, i * total_size_per_layer, sync_var_size_per_layer),
            boost::interprocess::mapped_region(shm_obj, boost::interprocess::read_write, i * total_size_per_layer + sync_var_size_per_layer, buffer_size_per_layer)
        ));

        if (is_server) {
            std::memcpy(mapped_regions.at(i).first.get_address(), (void*)(&binary_wait_cond), sync_var_size_per_layer);
        }
    }
}

SHM::~SHM() {
    if (is_server) {
        boost::interprocess::shared_memory_object::remove(name.c_str());
        std::cout << "Removed SHM" << std::endl;
    } else {
        std::cout << "Did not remove SHM" << std::endl;
    }
}

void SHM::write_bytes(size_t layer_num, const uint8_t* msg, const uint32_t msg_size) {
    auto& layer_mapped_regions = mapped_regions.at(layer_num);
#if DEBUG_MODE
    std::cout << "Writing msg to SHM (layer " + std::to_string(layer_num) + ") : " << std::endl;
    print_bytes(msg, msg_size);
#endif
    if (msg_size > layer_mapped_regions.second.get_size() - sizeof(uint32_t)) throw std::out_of_range("Message is too big for SHM");
    std::memcpy(layer_mapped_regions.second.get_address(), (void*)&msg_size, sizeof(uint32_t)); // First write the size of the message
    std::memcpy((uint8_t*)layer_mapped_regions.second.get_address() + sizeof(uint32_t), (void*)msg, msg_size); // Now write the message

    std::atomic_bool* sync_var_ptr = static_cast<std::atomic_bool*>(layer_mapped_regions.first.get_address());
    sync_var_ptr->store(binary_wait_cond);
}

bool SHM::read_bytes(size_t layer_num, uint8_t* msg, uint32_t& msg_size) {
    auto& layer_mapped_regions = mapped_regions.at(layer_num);
    std::atomic_bool* sync_var_ptr = static_cast<std::atomic_bool*>(layer_mapped_regions.first.get_address());

    while(keep_running && sync_var_ptr->load() == binary_wait_cond); // Wait until the read is allowed
    if (!keep_running) return false;

    msg_size = *(uint32_t*)layer_mapped_regions.second.get_address();
    std::copy(
        (uint8_t*)layer_mapped_regions.second.get_address() + sizeof(uint32_t),
        (uint8_t*)layer_mapped_regions.second.get_address() + sizeof(uint32_t) + msg_size,
        msg
    );

#if DEBUG_MODE
    std::cout << "Read msg from SHM: " << std::endl;
    print_bytes(msg, msg_size);
#endif
    return true;
}