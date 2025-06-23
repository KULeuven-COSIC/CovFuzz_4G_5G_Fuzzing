#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

/**
 * Enum containing all of the supported mutators
 */
enum mutator {
        random_mutator_field,
        add_mutator,
        sub_mutator,
        to_maximum_mutator,
        to_minimum_mutator,
        mutator_amount,
        unknown_mutator
};

NLOHMANN_JSON_SERIALIZE_ENUM( mutator, {
    {unknown_mutator, "UNKNOWN"},
    {random_mutator_field, "RAND"},
    {to_maximum_mutator, "MAX"},
    {to_minimum_mutator, "MIN"},
    {add_mutator, "ADD"},
    {sub_mutator, "SUB"}
})

const std::string get_mutator_name(mutator mut);
mutator get_mutator_by_name(const std::string& mut_name);

bool apply_mutator(mutator mut, uint64_t current_value, uint64_t max_value, uint64_t *new_value_);