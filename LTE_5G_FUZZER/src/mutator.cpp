#include "mutator.h"

#include "helpers.h"
#include "my_logger.h"
#include "statistics.h"

#include "Configs/Fuzzing_Settings/main_config.h"

extern Main_Config main_config_g;
extern My_Logger my_logger_g;
extern Statistics statistics_g;

const std::string get_mutator_name(mutator mut)
{
    if (mut == random_mutator_field) return "RANDOM MUTATOR";
    else if (mut == add_mutator) return "ADD MUTATOR";
    else if (mut == to_maximum_mutator) return "MAX MUTATOR";
    else if (mut == to_minimum_mutator) return "MIN MUTATOR";
    else if (mut == sub_mutator) return "SUB MUTATOR";
    return "UNKNOWN MUTATOR";
}

mutator get_mutator_by_name(const std::string& mut_name) {
        if (mut_name == "RANDOM MUTATOR") return random_mutator_field;
        else if (mut_name == "ADD MUTATOR") return add_mutator;
        else if (mut_name == "MAX MUTATOR") return to_maximum_mutator;
        else if (mut_name == "MIN MUTATOR") return to_maximum_mutator;
        else if (mut_name == "SUB MUTATOR") return sub_mutator;
        return unknown_mutator;
}

bool apply_mutator(mutator mut, uint64_t current_value, uint64_t max_value, uint64_t *new_value) {
        if (max_value == 0 || max_value == 1) {
                *new_value = current_value;
                my_logger_g.logger->warn("Too small max_value: {}", max_value);
                if (main_config_g.verbose) printf(RED "Too small max_value: %lu\n" CRESET, max_value);
                return false;
        }
        if (mut == random_mutator_field) {
                statistics_g.rand_mutator_counter++;
                uint64_t fuzzed_value = helpers::UR0(max_value);
	        //while (fuzzed_value == current_value && max_value > 1) fuzzed_value = UR0(max_value); //UNCOMMENT THIS LINE IF YOU WANT TO GUARANTEE THAT FUZZED VALUE IS DIFFERENT FROM THE INITIAL VALUE
                *new_value = fuzzed_value;
        } else if (mut == add_mutator) {
                //printf("Applying ADD mutation!\n");
                statistics_g.add_mutator_counter++;
                if (max_value - 1 <= current_value) *new_value = current_value; //Impossible to ADD anything
                else *new_value = current_value + 1;
        } else if (mut == sub_mutator) {
                statistics_g.sub_mutator_counter++;
                if (current_value == 0) *new_value = 0; // Impossible to SUB anything
                else *new_value = current_value - 1;
        } else if (mut == to_minimum_mutator) {
                statistics_g.min_mutator_counter++;
                *new_value = 0;
        } else if (mut == to_maximum_mutator) {
                statistics_g.max_mutator_counter++;
                *new_value = max_value - 1;
        } else {
                *new_value = current_value;
                my_logger_g.logger->warn("Mutator is not yet implemented!");
                if (main_config_g.verbose) printf(RED "Mutator is not yet implemented!\n" CRESET);
                return false;
        }
        return true;
}