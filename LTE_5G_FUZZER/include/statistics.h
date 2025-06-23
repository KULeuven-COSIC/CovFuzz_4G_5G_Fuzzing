#pragma once

#include <iostream>
#include <cstdlib>
#include <string>


struct Statistics {

    size_t attach_failed_counter = 0;
    size_t attach_successful_counter = 0;
    size_t hang_counter = 0;
    size_t failed_fuzz_iterations_counter = 0;
    bool has_this_iteration_failed = false;
    size_t restart_counter = 0;
    size_t empty_iterations = 0;

    size_t rand_mutator_counter = 0;
    size_t min_mutator_counter  = 0;
    size_t max_mutator_counter  = 0;
    size_t add_mutator_counter  = 0;
    size_t sub_mutator_counter  = 0;
    size_t injection_counter = 0;

    // Crashes
    size_t enb_crash_counter = 0;
    size_t epc_crash_counter = 0;
    size_t ue_crash_counter  = 0;
};