#include "Fuzzers/base_fuzzer.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"

#include "my_logger.h"
#include "packet.h"
#include "seed.h"

#include <iostream>
#include <map>
#include <memory>
#include <unordered_set>

extern My_Logger my_logger_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;

std::unordered_map<std::string, std::unordered_set<Packet, PacketHash> > Base_Fuzzer::packet_buffer;
std::shared_ptr<Seed> Base_Fuzzer::current_seed = std::make_shared<Seed>();
std::shared_ptr<Seed> Base_Fuzzer::previous_seed = std::make_shared<Seed>();
std::unordered_map<std::string, std::vector<Field> > Base_Fuzzer::packet_fields_database;
std::vector<Field*> Base_Fuzzer::mutated_fields;
size_t Base_Fuzzer::mutated_fields_num = 0;
Mutated_Field_Num_Tracker Base_Fuzzer::mut_field_num_tracker;

bool Base_Fuzzer::init()
{
	if (!fuzz_strategy_config_g.seed_paths.empty()) {
		for (const auto& seed_path : fuzz_strategy_config_g.seed_paths) {
			if (seed_path.empty()) continue;
			Seed s;
			if (!helpers::parse_json_file_into_class_instance(seed_path, s)) {
				my_logger_g.logger->error("Could not read a predefined seed from : {}", seed_path);
				return false;
			}
			auto patches = s.get_patches();
			predefined_patches.insert(predefined_patches.end(), patches.begin(), patches.end());
			my_logger_g.logger->info("Read {} patches from {}", predefined_patches.size(), seed_path);
		}
    }
    return true;
}

bool Base_Fuzzer::prepare_new_iteration() {
    previous_seed = current_seed;
	current_seed = std::make_shared<Seed>();
	return true;
}

bool Base_Fuzzer::apply_predefined_patches(Packet& packet)
{
	if (!predefined_patches.empty()) {
		auto it = std::find_if(predefined_patches.begin(), predefined_patches.end(), [&](const std::shared_ptr<Patch>& patch) {
			return ((patch->get_packet_summary_short() == packet.get_summary_short() && !packet.get_summary_short().empty())
					|| (patch->get_packet_summary() == packet.get_summary() && !packet.get_summary().empty()))
					&& (patch->get_layer() == packet.get_layer());
			});
		if (it != predefined_patches.end()) {
			std::shared_ptr<Patch>& patch = *it;
			if (!patch->apply(&packet)) {
				my_logger_g.logger->debug("Couldn't apply patch {}", patch->get_id());
				return false;
			}
			my_logger_g.logger->info("Applying predefined patches");
			log_patch(patch);
			return true;
		}
		my_logger_g.logger->debug("No predefined patches to the current packet");
	}
	my_logger_g.logger->debug("No predefined patches to apply");
    return true;
};

void print_field_mutation_probabilities() {
    printf("Mutation probabilities:\n");
    for (const auto& [name, vec_f] : Base_Fuzzer::packet_fields_database) {
        printf("%s:\n", name.c_str());
        for (const auto& f : vec_f) {
            printf("\t%s: %.2f\n", f.field_name.c_str(), f.mutation_probability);
        }
        printf("\n");
    }
}

void log_field_mutation_probabilities() {
    printf("Mutation probabilities:\n");
    for (const auto& [name, vec_f] : Base_Fuzzer::packet_fields_database) {
        my_logger_g.logger->debug("{}", name);
        for (const auto& f : vec_f) {
            my_logger_g.logger->debug("\t{}: {:2f}\n", f.field_name, f.mutation_probability);
        }
        printf("\n");
    }
}