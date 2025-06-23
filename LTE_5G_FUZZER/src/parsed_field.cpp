#include "parsed_field.h"

#include <bit>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <stdexcept>

parsed_field::parsed_field(const parsed_field& p) : offset(p.offset), length(p.length), mask(p.mask), mask_offset(p.mask_offset), mask_align(p.mask_align) {}

std::ostream& parsed_field::dump(std::ostream& os) const {
    os << "===PARSED FIELD===" << std::endl;
    os << "offset: " << offset << std::endl;
    os << "length: " << length << std::endl;
    os << "mask: " << std::bitset<64>(mask) << std::endl;
    os << "mask offset: " << mask_offset << std::endl;
    os << "mask align: " << mask_align << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const parsed_field& f) {
    return f.dump(os);
}

uint16_t get_field_size_bits(parsed_field *f) {
    if (f->length == 0) return 0;
	uint16_t result = f->length * 8; // Length in bits
	result -= f->mask_offset;
	result -= (uint8_t)__builtin_ctz(f->mask);
	return result;
}

// TODO: Revise this function
void set_field_value(std::vector<uint8_t>& v, parsed_field *pf, uint64_t new_value){
	uint64_t res = 0;
    for (size_t i = 0; i < 8; i++) {
        res <<= 8;
        if (pf->offset + i >= v.size()) {
            res <<= (8 * (7 - i));
            break;
        }
        res |= (v.at(pf->offset + i) & 0xFF);
    }
    uint64_t mask = pf->mask;
    uint64_t mask_align = pf->mask_align;
    int how_many_right_rotations = (8 - pf->length) * 8;
    res = std::rotr(res, how_many_right_rotations);
    if (mask_align){
        how_many_right_rotations += mask_align;
        res = std::rotr(res, mask_align);
        how_many_right_rotations -= 8;
        res = std::rotr(res, -8);
    }
    while (!(mask & 0x01)) {
        mask >>= 1;
        res = std::rotr(res, 1);
        how_many_right_rotations++;
    }
    int mask_length = 64 - __builtin_clzll(mask); 
    uint64_t set_to_zero_mask = ((~0UL) << mask_length);
    uint64_t cut_new_value_mask = ((~0UL) >> (64 - mask_length));
    new_value &=cut_new_value_mask;
    res &=set_to_zero_mask;
    res |= new_value;
    res = std::rotr(res, -how_many_right_rotations); //rotate back
    for (size_t i = 0; i < 8; i++) {
        if (pf->offset + i >= v.size()) break;
        v.at(pf->offset + i) = (uint8_t)((res >> ((7 - i) * 8)) & 0xFF);
    }
}

// TODO: Revise this function
uint64_t get_field_value(const std::vector<uint8_t>& v, parsed_field *pf) {
    if (pf->length == 0) throw std::runtime_error("Field has length 0!");
    uint64_t res = 0;
    for (size_t i = 0; i < 8; i++) {
        res <<= 8;
    if (pf->offset + i >= v.size()) {
            res <<= (8 * (7 - i));
            break;
        }
        res |= (v.at(pf->offset + i) & 0xFF);
    }
    uint64_t mask = pf->mask;
    if (pf->mask_align){
        res >>= pf->mask_align;
        res <<= 8;
    }
    res >>= ((8 - pf->length) * 8);
    res &= mask;
    while (!(mask & 0x01)) {
        mask >>= 1;
        res >>= 1;
    }
    return res;
}