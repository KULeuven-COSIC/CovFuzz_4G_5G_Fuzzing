#pragma once

#include "common_utils.h"

#include <atomic>
#include <memory>
#include <utility>
#include <vector>
#include <cstddef>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

size_t get_random_bytes(uint8_t* arr, size_t max_size);
void print_bytes(const uint8_t* arr, size_t size);

class SHM {

public:
    SHM(bool is_server, const std::string name, size_t buffer_size_per_layer, size_t sync_var_size_per_layer = sizeof(std::atomic_bool), size_t layers_num = EnumMutex::SHM_MUTEX_MAX);
    ~SHM();

    void write_bytes(size_t layer_num, const uint8_t*, const uint32_t);
    bool read_bytes(size_t layer_num, uint8_t*, uint32_t&);

    static volatile int keep_running;

private:

    bool is_server;
    bool binary_wait_cond;
    const std::string name;

    size_t total_size_per_layer;
    size_t buffer_size_per_layer;
    size_t sync_var_size_per_layer;

    size_t layers_num;

    boost::interprocess::shared_memory_object shm_obj;
    std::vector< std::pair<boost::interprocess::mapped_region, boost::interprocess::mapped_region> > mapped_regions;

};

