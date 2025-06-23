#include "Fuzzers/random_fuzzer.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "my_logger.h"
#include "statistics.h"

extern Main_Config main_config_g;
extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern My_Logger my_logger_g;
extern Statistics statistics_g;

bool RandomFuzzer::init() {
	return Base_Fuzzer::init();
}

bool RandomFuzzer::fuzz(Packet& packet) {
	
	if (!helpers::is_fuzzing_direction(packet.get_direction())) {
        return true;
    }

	if (!apply_predefined_patches(packet)) return false;

	if (!helpers::is_state_being_fuzzed(packet.get_summary_short())) return true; // Skip fuzzing

	size_t rn = helpers::UR0(100); // Generate random number

	if (rn < fuzz_strategy_config_g.skip_prob) {
		/* Do not fuzz */
		return true;
	} else if (rn < (fuzz_strategy_config_g.mutation_prob + fuzz_strategy_config_g.skip_prob)) {// || packet.get_layer() == 0) { //If MAC layer
		//Fuzz randomly
		if (packet.get_fields().size() == 0) {
			/* Nothing to fuzz. Create an empty patch. */
			std::shared_ptr<Patch> new_empty_patch = std::make_shared<Patch>(packet.get_channel(), packet.get_layer(), packet.get_direction(), packet.get_summary(), packet.get_summary_short());
			current_seed->add_patch(std::move(new_empty_patch));
		} else {
			//Create random mutations and save them to the patch
			std::shared_ptr<Patch> patch = std::make_shared<Patch>(packet.get_channel(), packet.get_layer(), packet.get_direction(), packet.get_summary(), packet.get_summary_short());
			try {
				patch->add_mutations(create_mutations(Base_Fuzzer::packet_fields_database.at(packet.get_summary_short() + std::to_string(packet.get_data().size()))));
			} catch (std::exception& ex) {
				my_logger_g.logger->debug("Exception happened: {}", ex.what());
				return false;
			}

			my_logger_g.logger->debug("Created new patch with id={}", patch->get_id());
			if (!patch->apply(&packet)) {
				if (main_config_g.verbose) printf(RED "Applying patch failed in random fuzzer.\n" CRESET);
				my_logger_g.logger->error("Applying patch failed in random fuzzer.\nPacket: {}\nPatch{}\n", packet, *patch);
				return false;
			}
			//LOG
			log_patch(patch);

			current_seed->add_patch(std::move(patch));
		}

	} else {
		// Inject a different message
		try {
			std::unordered_set<Packet, PacketHash>& packets = packet_buffer.at(packet.get_dissector_name());
			if (packets.size() != 0) {
				size_t rn = helpers::EXP0(packets.size()); // Pick a packet
				auto p = std::next(packets.begin(), packets.size() - rn - 1); // count from behind; goal is to choose newer packets more frequently
				if (packet.get_summary_short() == p->get_summary_short()) {
					/* Picked the packet with the same type */
					std::shared_ptr<Patch> new_empty_patch = std::make_shared<Patch>(packet.get_channel(), packet.get_layer(), packet.get_direction(), packet.get_summary(), packet.get_summary_short());
					current_seed->add_patch(std::move(new_empty_patch));
			
					my_logger_g.logger->debug("Chose the same packet type for injection. Keeping the packet unchanged.");
					return true;
				}

				my_logger_g.logger->debug("Injection was successful");
				std::shared_ptr<Patch> patch = std::make_shared<Patch>(packet.get_channel(), packet.get_layer(), packet.get_direction(), packet.get_summary(), packet.get_summary_short(), *p);
				
				my_logger_g.logger->debug("Created new patch with id={}", patch->get_id());
				if (!patch->apply(&packet)) {
					std::shared_ptr<Patch> new_empty_patch = std::make_shared<Patch>(packet.get_channel(), packet.get_layer(), packet.get_direction(), packet.get_summary(), packet.get_summary_short());
					current_seed->add_patch(std::move(new_empty_patch));
					if (main_config_g.verbose) printf(RED "Applying patch failed in random fuzzer.\n" CRESET);
					my_logger_g.logger->error("Applying patch failed in random fuzzer. \nPacket: {}\nPatch{}\n", packet, *patch);
					return false;
				}

				//LOG
				log_patch(patch);
				current_seed->add_patch(std::move(patch));
				statistics_g.injection_counter++;
				
			} else {
				std::shared_ptr<Patch> new_empty_patch = std::make_shared<Patch>(packet.get_channel(), packet.get_layer(), packet.get_direction(), packet.get_summary(), packet.get_summary_short());
				current_seed->add_patch(std::move(new_empty_patch));
				my_logger_g.logger->debug("Injection: packets.size() == 0");
			}
		} catch(const std::exception& ex) {
			my_logger_g.logger->debug("Exception happened {}", ex.what());
			return false;
		}
	}
	
	return true;
}