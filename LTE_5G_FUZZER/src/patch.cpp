#include "patch.h"

#include "field.h"
#include "helpers.h"
#include "mutation.h"
#include "my_logger.h"
#include "packet.h"

#include "Configs/Fuzzing_Settings/main_config.h"

 

#include <iostream>
#include <string>
#include <vector>

uint64_t Patch::total_patches = 0;
extern Main_Config main_config_g;
extern My_Logger my_logger_g;

Patch::Patch(CHANNEL channel, uint8_t layer, uint8_t direction, const std::string& packet_summary, const std::string& packet_summary_short) 
    : id(total_patches++), channel(channel), layer(layer), layer_name(helpers::get_layer_name_by_idx(layer)), direction(direction), 
    packet_summary(packet_summary), packet_summary_short(packet_summary_short)  {}

Patch::Patch(std::vector<std::shared_ptr<Mutation>> mutations, CHANNEL channel, uint8_t layer, uint8_t direction,
        const std::string& packet_summary, const std::string& packet_summary_short) 
        : mutations(mutations),  id(total_patches++), channel(channel), layer(layer), layer_name(helpers::get_layer_name_by_idx(layer)), 
        direction(direction), packet_summary(packet_summary), packet_summary_short(packet_summary_short), patch_type(PatchType::MutationPatch)  {}

Patch::Patch(const Patch& patch) : iteration(patch.iteration), mutations(patch.mutations), id(total_patches++), 
    channel(patch.channel), layer(patch.layer), layer_name(helpers::get_layer_name_by_idx(patch.layer)), direction(patch.direction), 
    packet_summary(patch.packet_summary), packet_summary_short(patch.packet_summary_short), old_packet(patch.old_packet), 
    new_packet(patch.new_packet), patch_type(patch.patch_type) {}

Patch::Patch(CHANNEL channel, uint8_t layer, uint8_t direction, const std::string& packet_summary, const std::string& packet_summary_short, 
    const Packet& new_packet) : id(total_patches++), channel(channel), layer(layer), layer_name(helpers::get_layer_name_by_idx(layer)), 
    direction(direction), packet_summary(packet_summary), packet_summary_short(packet_summary_short), new_packet(new_packet), 
    patch_type(PatchType::InjectionPatch)  {}

Patch& Patch::operator=(const Patch& p) {
        iteration = p.iteration;
        channel = p.channel;
        layer = p.layer;
        direction = p.direction;
        packet_summary = p.packet_summary;
        packet_summary_short = p.packet_summary_short;
        layer_name = p.layer_name;
        patch_type = p.patch_type;
        old_packet = p.old_packet;
        new_packet = p.new_packet;
        lives_left = p.lives_left;
        return (*this);
}


bool Patch::apply(Packet *packet)
{

    if (layer != packet->get_layer())
    {
        if (main_config_g.verbose) std::cerr << RED << "Patch not applicable! Patch layer != Packet layer: " << layer_name << " != " << helpers::get_layer_name_by_idx(packet->get_layer()) << CRESET << std::endl;
        my_logger_g.logger->debug("Patch not applicable! Patch layer != Packet layer: {} != {}", layer_name, helpers::get_layer_name_by_idx(packet->get_layer()));
        return false;
    }
    if (!packet_summary.empty())
    {
        if (packet_summary != packet->get_summary())
        {
            if (main_config_g.verbose)
                printf(RED "Patch not applicable! Patch packet_summary!= Packet packet_summary: %s != %s\n" CRESET, packet_summary.c_str(), packet->get_summary().c_str());
            my_logger_g.logger->debug("Patch not applicable! Patch packet_summary != Packet packet_summary: {} != {}", packet_summary, packet->get_summary());
            return false;
        }
    }
    else
    {
        if (packet_summary_short != packet->get_summary_short())
        {
            if (main_config_g.verbose)
                printf(RED "Patch not applicable! Patch packet_summary_short != Packet packet_summary_short: %s != %s\n" CRESET, packet_summary_short.c_str(), packet->get_summary_short().c_str());
            my_logger_g.logger->debug("Patch not applicable! Patch packet_summary_short != Packet packet_summary_short: {} != {}", packet_summary_short, packet->get_summary_short());
            return false;
        }
    }

    if (this->is_empty_patch())
        return true;

    if (patch_type == PatchType::InjectionPatch)
    {
        if (!new_packet.get_data().empty())
        {
            old_packet = *packet;
            *packet = new_packet;
            return true;
        }
        else
        {
            my_logger_g.logger->warn("Tried to apply injection patch with empty new_packet field");
            return false;
        }
    }

    if (packet->get_fields().size() == 0)
    {
        if (!is_empty_patch())
        {
            if (main_config_g.verbose)
                printf(RED "Applying non-empty patch to unresolved packet!\n" CRESET);
            my_logger_g.logger->debug("Applying non-empty patch to unresolved packet!");
            return false;
        }
    }
    old_packet = *packet;
    for (const auto &mutation : mutations)
    {
        if (!mutation->prepare(*packet))
        {
            return false;
        }
    }
    for (const auto &mutation : mutations)
    {
        mutation->apply(packet);
    }
    new_packet = *packet;
    return true;
}

// Visualize the patch
std::ostream& Patch::dump(std::ostream& os) const {
        std::vector<parsed_field*> fuzzed_fields;
        os << RED << "================ PATCH ID: %" << id << " ================" << CRESET<< std::endl;
        os << YELLOW << "Iteration: " << iteration << CRESET << std::endl;
        os << RED << "Patch layer: " << layer_name << CRESET << std::endl;
        os << YELLOW << "Patch type: " << patch_type << CRESET << std::endl;
        if (is_empty_patch()) {
            os << "PATCH IS EMPTY!" << std::endl;
        } else if (patch_type == PatchType::InjectionPatch) {
            os << YELLOW << "Injection patch" << CRESET << std::endl;
        } else {
            os << "TOTAL MUTATORS: " << mutations.size() << std::endl;
            for (auto mut_ptr : mutations) {
                mut_ptr->dump(os);
            }
        }
        if (!packet_summary.empty()) {
                os << RED << "MESSAGE TYPE: " << packet_summary << CRESET << std::endl;
        } else if (!packet_summary_short.empty()) {
                os << RED "MESSAGE TYPE: " << packet_summary_short << CRESET << std::endl;
        }
        os << RED << "OLD PACKET: ";
        old_packet.dump_fuzzed_packet(os, fuzzed_fields);
        os << RED << "\nNEW PACKET: ";
        new_packet.dump_fuzzed_packet(os, fuzzed_fields);
        os << std::endl;
        os << RED << "================ END OF PATCH ID: %" << id << " ================\n" << CRESET << std::endl;
        return os;
}

Packet& Patch::get_old_packet() {
        return old_packet;
}

Packet& Patch::get_new_packet() {
        return new_packet;
}

uint64_t Patch::get_id() const {
        return id;
}

uint8_t Patch::get_direction() const {
        return direction;
}

bool Patch::is_empty_patch() const {
        if (patch_type == PatchType::MutationPatch) return mutations.size() == 0 ? true : false;
        return new_packet.get_data().empty();
}

void Patch::add_mutations(const std::vector<std::shared_ptr<Mutation>>& mutations) {
        this->mutations.insert(this->mutations.end(), mutations.begin(), mutations.end());
}

void Patch::add_mutation(const std::shared_ptr<Mutation>& mutation) {
        mutations.push_back(mutation);
}


const size_t Patch::get_mutations_size() const {
        return mutations.size();
}

const std::vector<std::shared_ptr<Mutation>>& Patch::get_mutations() const {
        return mutations;
}

const CHANNEL Patch::get_channel() const {
        return channel;
}

const uint8_t Patch::get_layer() const {
        return layer;
}

const std::string Patch::get_layer_name() const {
        return layer_name;
}

const Packet& Patch::get_old_packet() const {
        return old_packet;
}

const Packet& Patch::get_new_packet() const {
        return new_packet;
}

const std::string Patch::get_packet_summary() const {
        return packet_summary;
}

const std::string Patch::get_packet_summary_short() const {
        return packet_summary_short;
}

void Patch::delete_all_mutations() {
        mutations.clear();
}

bool Patch::operator==(const Patch& p) const {
        bool ret = (packet_summary_short == p.packet_summary_short) && (layer == p.layer) && (p.get_mutations_size() == mutations.size());
        if (ret) {
            for (size_t i = 0; i < mutations.size(); i++) {
                if (*(p.mutations.at(i).get()) != *(mutations.at(i).get())) break;
            }   
        }
        return ret;
}

std::ostream& operator<<(std::ostream& os, const Patch& patch) {
        os << "================ PATCH ID: " <<  patch.get_id() << " ================\n";
        os << "Patch layer: " << patch.get_layer_name() << "\n";
        os << "Patch type: " << patch.get_patch_type() << "\n";
        if (patch.is_empty_patch()) {
                os << "PATCH IS EMPTY!\n";
        } else if (patch.get_patch_type() == PatchType::InjectionPatch) {
            os << "Injection patch\n";
        } else {
                os << "TOTAL MUTATORS: " << patch.get_mutations().size() << "\n";
                for (auto mut_ptr : patch.get_mutations()) {
                        os << mut_ptr;
                        //fuzzed_fields.push_back(&(mut_ptr->parsed_f));
                }
        }
        os << "MESSAGE TYPE: ";
        if (!patch.get_packet_summary().empty()) os << patch.get_packet_summary();
        else if (!patch.get_packet_summary_short().empty()) os << patch.get_packet_summary_short();
        os << "\n"; //pdu_type_to_string(type).c_str());
        os << "OLD PACKET: " << patch.get_old_packet() << "\n";
        os << "NEW PACKET: " << patch.get_new_packet() << "\n";
        os << "================ END OF PATCH ID: " << patch.get_id() << " ================\n\n";

        return os;
}

void log_patch(const std::shared_ptr<Patch>& patch) {
    for (size_t i = 0; i < patch->get_mutations_size(); i++) {
        my_logger_g.logger->info("Mutator: {}; Fuzzed field {} (index={}): Old value={}; New value={}; Iteration={}", get_mutator_name(patch->get_mutations().at(i)->get_mutator()), patch->get_mutations().at(i)->field.field_name, patch->get_mutations().at(i)->field.index, patch->get_mutations().at(i)->get_old_value(), patch->get_mutations().at(i)->get_new_value(), patch->iteration);
    }
}