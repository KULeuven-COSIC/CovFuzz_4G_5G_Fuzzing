#include "mutated_field_num_tracker.h"

#include <numeric>

void Mutated_Field_Num_Tracker::push_mutated_field_num(std::size_t mut_field_num) {
    mutated_fields_stat.push_back(mut_field_num);
}

bool Mutated_Field_Num_Tracker::needs_reset() {
    if (mutated_fields_stat.capacity() != mutated_fields_stat.size()) return false;
    int num_of_mutated_iterations = 0;
    for (auto mut : mutated_fields_stat) {
        if (mut > 0) num_of_mutated_iterations++;
    }

    const double th = 0.5; // If half or less iterations mutate nothing --> reset the probabilities
    if (num_of_mutated_iterations / static_cast<double>(mutated_fields_stat.size()) <= th) return true;
    return false;
}

void Mutated_Field_Num_Tracker::reset() {
    mutated_fields_stat.clear();
}