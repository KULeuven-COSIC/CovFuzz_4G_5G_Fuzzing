#pragma once

#include "mutator.h"

#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

enum class FuzzingStrategy {
	NONE,
	RANDOM_FUZZING,
	LAST_FUZZING_STRATEGY
};

NLOHMANN_JSON_SERIALIZE_ENUM( FuzzingStrategy, {
    {FuzzingStrategy::NONE, "NONE"},
	{FuzzingStrategy::RANDOM_FUZZING, "RANDOM_FUZZING"}
})

std::string get_fuzzing_strategy_name_by_idx(FuzzingStrategy fs);
std::string get_all_fuzzing_strategy_names(const std::vector<FuzzingStrategy>& fsv);

class Fuzz_Strategy_Config {
public:

    /* Global fuzzing strategy */
    int attach_attempts_per_iteration; /* The length of the fuzzing */
    int restart_after_consecutive_failed_attaches = -1;
    int restart_after_consecutive_successful_attaches = -1;
    int restart_after_consecutive_hangs = -1;
    int restart_after_consecutive_empty_iterations = 5;


    std::vector<FuzzingStrategy> fuzzing_strategies; /* The list of strategies to be applied every iteration */

    bool use_existing_patches = false;
    std::string patches_path = ""; // Ignored if use_existing_patches = false

    bool use_existing_seeds = false;
    std::vector<std::string> seed_paths = {}; // Ignored if use_existing_seeds = false

    /* Coverage settings */
    bool use_coverage_feedback = false;
    bool use_buckets = true; // Ignored if use_coverage_feedback = false
    bool use_coverage_logging = true;
    std::string coverage_log_path = "";

    /* Probability optimization settings */
    bool use_probability_resets = true;
    bool use_probability_optimization = true;
    double field_max_mut_prob = 0.90;
    double field_min_mut_prob = 0.005;
    float attenuation_coefficient = 1.0;
    size_t mutation_prob = 80;
    size_t skip_prob = 0;
    double init_prob_mult_factor = 3.0;
    double beta = 2.9;
    double beta_2 = 0.0;

    /* Mutators to be used while fuzzing */
    std::vector<mutator> permitted_mutators = {
        random_mutator_field,
        add_mutator,
        sub_mutator,
        to_maximum_mutator,
        to_minimum_mutator
    };

    // State fuzzing options
    size_t iterations_per_state = 0;
    std::string state_to_start_fuzz_from = "";
    std::vector<std::string> states_to_fuzz = {};
    std::vector<std::string> states_not_to_fuzz = {};

public:
    std::ostream& dump(std::ostream& os) const;
};

std::ostream& operator<< (std::ostream& os, const Fuzz_Strategy_Config& path_config);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Fuzz_Strategy_Config, 
    attach_attempts_per_iteration, 
    restart_after_consecutive_failed_attaches, 
    restart_after_consecutive_successful_attaches, 
    restart_after_consecutive_hangs, 
    restart_after_consecutive_empty_iterations,
    fuzzing_strategies,
    use_existing_patches,
    patches_path,
    use_existing_seeds,
    seed_paths,
    use_coverage_feedback,
    use_buckets,
    use_coverage_logging,
    coverage_log_path,
    use_probability_resets,
    use_probability_optimization,
    attenuation_coefficient,
    mutation_prob,
    skip_prob,
    init_prob_mult_factor,
    beta,
    beta_2,
    permitted_mutators,
    iterations_per_state,
    state_to_start_fuzz_from,
    states_to_fuzz,
    states_not_to_fuzz
)