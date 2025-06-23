#pragma once

#include <cstddef>

#include <boost/circular_buffer.hpp>

class Mutated_Field_Num_Tracker {
public:
    Mutated_Field_Num_Tracker() {
        mutated_fields_stat.set_capacity(10);
    }

    void push_mutated_field_num(std::size_t mut_field_num);

    bool needs_reset();

    void reset();

private:
    boost::circular_buffer<std::size_t> mutated_fields_stat;
};