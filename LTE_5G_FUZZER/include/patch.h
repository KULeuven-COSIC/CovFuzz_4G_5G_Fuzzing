#pragma once

#include "helpers.h"
#include "field.h"
#include "mutation.h"
#include "packet.h"

#include "common_utils.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

enum PatchType {
    MutationPatch,
    InjectionPatch
};

NLOHMANN_JSON_SERIALIZE_ENUM( PatchType, {
    {InjectionPatch, "InjectionPatch"},
    {MutationPatch, "MutationPatch"}
})

class Patch { //Patch is applied to a particular message channel, so we are sure that it contains the field we want to mutate
public:
    Patch() = default;
    Patch(CHANNEL channel, uint8_t layer, uint8_t direction, const std::string& packet_summary, const std::string& packet_summary_short);
    Patch(std::vector<std::shared_ptr<Mutation>> mutations, CHANNEL channel, uint8_t layer, uint8_t direction,
         const std::string& packet_summary, const std::string& packet_summary_short);
    Patch(const Patch& patch);
    Patch(CHANNEL channel, uint8_t layer, uint8_t direction, const std::string& packet_summary, const std::string& packet_summary_short, 
        const Packet& new_packet);
    Patch& operator=(const Patch& p);

public:
    bool apply(Packet *packet);
    // Visualize the patch
    std::ostream& dump(std::ostream& os) const;
    Packet& get_old_packet();
    Packet& get_new_packet();
    uint64_t get_id() const;
    bool is_empty_patch() const;
    void add_mutations(const std::vector<std::shared_ptr<Mutation>>& mutations);
    void add_mutation(const std::shared_ptr<Mutation>& mutation);
    const size_t get_mutations_size() const;
    const std::vector<std::shared_ptr<Mutation>>& get_mutations() const;
    const CHANNEL get_channel() const;
    const uint8_t get_layer() const;
    const std::string get_layer_name() const;
    const Packet& get_old_packet() const;
    const Packet& get_new_packet() const;
    const std::string get_packet_summary() const;
    const std::string get_packet_summary_short() const;
    void delete_all_mutations();

    inline PatchType get_patch_type() const {
        return patch_type;
    }

    uint8_t get_direction() const;

    bool operator==(const Patch& p) const;

    friend class PatchHasher;

    size_t lives_left = 3;
    int iteration = -1;

private:
    std::vector<std::shared_ptr<Mutation>> mutations; // stack of mutations to be applied to the packet
    static uint64_t total_patches;
    uint64_t id = 0; // patch id
    
    CHANNEL channel; // RRC Message channel
    uint8_t layer;
    std::string layer_name;

    uint8_t direction = DIRECTION_DOWNLINK;

    std::string packet_summary = "";
    std::string packet_summary_short = "";

    Packet old_packet;
    Packet new_packet;

    PatchType patch_type = PatchType::MutationPatch;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Patch, id, layer, layer_name, direction, mutations, packet_summary, packet_summary_short,
        old_packet, new_packet, lives_left, patch_type, iteration)
};

std::ostream& operator<<(std::ostream& os, const Patch& patch);

void log_patch(const std::shared_ptr<Patch>& patch);

class PatchHasher {
public:

    size_t operator() (const Patch& p) const {
        size_t h = std::hash<std::string>()(p.packet_summary_short);
        h ^= std::hash<int>()(p.layer);
        for (const std::shared_ptr<Mutation>& mut : p.mutations) {
            h ^= this->operator()(*mut);
        }
        return h;
    }

    size_t operator() (const std::shared_ptr<Patch>& ptr) const {
        return this->operator()(*ptr);
    }

    size_t operator() (const Mutation& m) const {
        return this->operator()(m.field) ^ std::hash<uint32_t>()(m.new_value);
    }

    size_t operator() (const Field& f) const {
        return (std::hash<std::string>()(f.field_name) ^ std::hash<int>()(f.index));
    }
};