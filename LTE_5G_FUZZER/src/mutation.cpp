#include "mutation.h"

//#include "base_coordinator.h" // TODO: Get rid of that
#include "field.h"
#include "helpers.h"
#include "mutator.h"
#include "my_logger.h"
#include "statistics.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Configs/Fuzzing_Settings/main_config.h"

#include "Fuzzers/base_fuzzer.h"

#include <algorithm>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

extern Main_Config main_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern Statistics statistics_g;
extern My_Logger my_logger_g;

uint64_t Mutation::total_mutations = 0;

Mutation::Mutation(mutator mut, Field f) {
        mutator_ = mut;
        mutator_name = get_mutator_name(mut);
        field = f;
        id = total_mutations++;
        is_completed = false;
}

Mutation::Mutation(const Mutation& mutation) {
        mutator_ = mutation.mutator_;
        mutator_name = mutation.mutator_name;
        field = mutation.field;
        old_value = mutation.old_value;
        new_value = mutation.new_value;
        id = mutation.id;
        is_completed = mutation.is_completed;
}

bool Mutation::operator==(const Mutation& mut) const {
        return mut.mutator_ == mutator_ && mut.field == field;
}

bool Mutation::operator!=(const Mutation& mut) const {
        return !(this->operator==(mut));
}

bool Mutation::prepare(const Packet& packet) {

        if (field.parsed_f.is_empty()) {
                const std::vector<Field>& packet_fields = packet.get_fields();
                if (auto it = std::find_if(packet_fields.begin(), packet_fields.end(), [&](const Field& f){
                        return f.field_name == field.field_name && f.index == field.index;
                }); it != packet_fields.end()) {
                        field.parsed_f = it->parsed_f;
                } else {
                        my_logger_g.logger->info("Cannot find the parsed_f (field_name={}, idx={})", field.field_name, field.index);
                        my_logger_g.logger->info("Only those fields are avaliable: {}", packet_fields);
                        failed = true;
                        return false;
                }
        }

        if (is_completed) return true;

        // Extract and save an old value
        uint8_t field_len = get_field_size_bits(&field.parsed_f);
        if (field_len == 0 || field_len >= 64) {
                my_logger_g.logger->warn("Field {} (index {}) has length {}", field.field_name, field.index, field_len);
                failed = true;
                return false;
        }
        uint64_t value;
        try {
                value = get_field_value(packet.get_data(), &field.parsed_f);
                old_value = value;
        } catch(std::exception& ex) {
                my_logger_g.logger->warn("Exception ({}) happened in get_field_value. Wanted to find field {} in packet {}", ex.what(), field, packet);
                failed = true;
                return false;
        }
        // Apply mutator to the old value and get a new one
        uint64_t max_value = (1UL << field_len); // Does NOT work without UL appendix, as results in a overflow of 32-bit number
        if (!apply_mutator(mutator_, value, max_value, &new_value)) return false;
        is_completed = true;
        return true;
}

void Mutation::apply(Packet* packet) {
        if (old_value != new_value) set_field_value(packet->get_data_ref(), &field.parsed_f, new_value); // Else there is no point in doing that
}

uint64_t Mutation::get_old_value() const {
        return old_value;
}

uint64_t Mutation::get_new_value() const {
        return new_value;
}

mutator Mutation::get_mutator() const {
        return mutator_;
}

std::ostream& Mutation::dump(std::ostream& os) const {
        os << "========= Mutation ID: " << id << " ================\n";
        if (failed) os << "MUTATION FAILED!\n";
        os << "Mutated field: " << field.field_name << "\n";
        os << "Mutator: " << mutator_name << "\n";
        os << "Old value: " << old_value << "\n";
        os << "New value: " << new_value << "\n";
        os << "========= END OF MUTATOR ID: " << id << " ================";
        return os;
}

std::ostream& operator<<(std::ostream& os, const Mutation& mutation) {
        return mutation.dump(os);
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Mutation>& mutation) {
        return mutation->dump(os);
}

void to_json(nlohmann::json& j, const Mutation& mut) {
        j = nlohmann::json{{"id", mut.id}, {"field", mut.field}, {"mutator_name", mut.mutator_name}, {"old_value", mut.old_value}, {"new_value", mut.new_value}, {"is_completed", mut.is_completed}};
}

void from_json(const nlohmann::json& j, Mutation& mut) {
        if (j.find("id") != j.end()) j.at("id").get_to(mut.id);
        else mut.id = 0;
        j.at("field").get_to(mut.field);
        if (j.find("mutator_name") != j.end()) {
                j.at("mutator_name").get_to(mut.mutator_name);
                mut.mutator_ = get_mutator_by_name(mut.mutator_name);
        } else {
                mut.mutator_name = "UNKNOWN";
                mut.mutator_ = unknown_mutator;
        }
        if (j.find("old_value") != j.end()) j.at("old_value").get_to(mut.old_value);
        if (j.find("new_value") != j.end()) j.at("new_value").get_to(mut.new_value);

        if (j.find("is_completed") != j.end()) {
                j.at("is_completed").get_to(mut.is_completed);
        } else {
                if (j.find("new_value") != j.end()) mut.is_completed = true;
                else mut.is_completed = false;   
        }
}

std::vector<std::shared_ptr<Mutation>> create_mutations(std::vector<Field>& fields) {
        std::vector<std::shared_ptr<Mutation>> result;
        for (size_t i = 0; i < fields.size(); i++) {
                if (helpers::URD(0.0, 1.0) < fields.at(i).mutation_probability) {
                        Base_Fuzzer::mutated_fields.push_back(&fields.at(i));
                        Base_Fuzzer::mutated_fields_num++;
                        size_t rnd_mut_idx = static_cast<size_t>(helpers::UR0(fuzz_strategy_config_g.permitted_mutators.size()));
                        result.push_back(std::make_shared<Mutation>((mutator)fuzz_strategy_config_g.permitted_mutators.at(rnd_mut_idx), fields.at(i)));
                }
        }
        //Base_Coordinator::mutated_fields.push_back(nullptr);
        return result;
}