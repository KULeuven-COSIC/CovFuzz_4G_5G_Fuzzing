#include "field.h"

#include "parsed_field.h"

#include <iomanip>
#include <iostream>

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"

#include "my_logger.h"

extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern My_Logger my_logger_g;

Field::Field(std::string field_name, parsed_field parsed_f) : field_name(field_name), parsed_f(parsed_f) {
    uint8_t field_len = get_field_size_bits(&parsed_f);
    my_logger_g.logger->debug("Init field {}: len={}, align={}, offset={}, mask={:x}, mask_offset={}", field_name, parsed_f.length, parsed_f.mask_align, parsed_f.offset, parsed_f.mask, parsed_f.mask_offset);
    // TODO: What if the field is too long?
    if (field_len == 0 || field_len >= 64) {
        my_logger_g.logger->warn("Field {} has length {}", field_name, field_len);
    } else {
        max_value = (1 << field_len);
    }
}

void Field::set_mutation_probability(double prob) {
    if (prob > fuzz_strategy_config_g.field_max_mut_prob) mutation_probability = fuzz_strategy_config_g.field_max_mut_prob;
    else if (prob < fuzz_strategy_config_g.field_min_mut_prob) mutation_probability = fuzz_strategy_config_g.field_min_mut_prob;
    else mutation_probability = prob;
}

std::ostream& Field::dump(std::ostream& os) const {
    os << "===FIELD===" << std::endl;
    os << "Name: " << field_name << std::endl;
    os << "Index: " << index << std::endl; ;
    os << "Mutation probability: " << std::fixed << std::setprecision(2) << mutation_probability << std::endl;
    os << parsed_f << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Field& f) {
    return f.dump(os);
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Field>& f) {
    return f->dump(os);
}