#pragma once

#include "field.h"
#include "packet.h"
#include "seed.h"

#include "mutated_field_num_tracker.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class Base_Fuzzer {
public:

    virtual ~Base_Fuzzer() {}

    virtual bool init();
    virtual bool fuzz(Packet&) = 0;
    virtual bool prepare_new_iteration();
    virtual bool apply_predefined_patches(Packet&);

public:

    inline void set_seed_path(const std::filesystem::path& sp) {
        seed_path = sp;
    }

    static std::unordered_map<std::string, std::unordered_set<Packet, PacketHash> > packet_buffer; // Buffer used to replay the previous messages
    
    static std::shared_ptr<Seed> current_seed; // Current seed OR (in case of repeat fuzz) seed to repeat
    static std::shared_ptr<Seed> previous_seed; // Helpful in detecting the cause of hangs

    // MAP: packet_name: all_fields
    static std::unordered_map<std::string, std::vector<Field> > packet_fields_database;
    /* Vector of pointers to the mutated fields. NB! In order to distinguish the fields that belong to different packets, there is a nullptr separator */
    static std::vector<Field*> mutated_fields;
    /* Amount of mutated fields in 1 iteration */
    static size_t mutated_fields_num;

    static Mutated_Field_Num_Tracker mut_field_num_tracker;

protected:
    std::filesystem::path seed_path;
    std::vector<std::shared_ptr<Patch>> predefined_patches;
};

void print_field_mutation_probabilities();
void log_field_mutation_probabilities();