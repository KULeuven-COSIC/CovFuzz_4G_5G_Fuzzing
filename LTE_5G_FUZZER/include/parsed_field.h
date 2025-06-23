#pragma once

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

/**
 * Structure representing the parsed field. 
 * It provides all the neccessary information to find the field in the packet.
 * The instance of the parsed_field is tied to the packet.
 */
struct parsed_field {
    parsed_field() = default;
    parsed_field(const parsed_field& p);

    //! Field's offset in bytes from the start of the packet.
	uint16_t offset = 0;
    //! Lengh of the field in bytes, i.e. from which byte to which byte the field spans over.
	uint8_t length = 0;
    //! Mask to extract the field's value from the corresponding bytes. Note, that the field are rarely byte-aligned.
	uint64_t mask = 0;
    //! Bit offset of the mask from the start of the first byte.
	uint16_t mask_offset = 0;
    //! Alignment to take into account before applying the mask.
    uint8_t mask_align = 0;

    inline bool is_empty() {
        return !(offset | length | mask | mask_offset);
    }

    //! Visualize the class instance
    std::ostream& dump(std::ostream& os) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(parsed_field, offset, length, mask, mask_offset, mask_align)

std::ostream& operator<<(std::ostream& os, const parsed_field& f);

//! Helper function to get the field's size in bits
uint16_t get_field_size_bits(parsed_field *f);

/**
 * Function to set value for a specific field
 * @param v Packet to set the value to
 * @param pf Parsed_field instance containing the information about the field
 * @param new_value Value to be set
 */
void set_field_value(std::vector<uint8_t>& v, parsed_field *pf, uint64_t new_value);

/**
 * Function to extract the field's value
 * @param v Packet to extract the field's value from
 * @param pf Parsed_field instance containing the information about the field
 * @return The extracted value 
 */
uint64_t get_field_value(const std::vector<uint8_t>& v, parsed_field *pf);