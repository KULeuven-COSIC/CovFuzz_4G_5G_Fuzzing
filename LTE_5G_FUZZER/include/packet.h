#pragma once

#include "common_utils.h"
#include "field.h"

#include "Protocol_Stack/Protocol_type.h"

#include "wdissector.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

class Packet {
public:
    Packet();
    Packet(uint8_t* data, uint16_t size, CHANNEL channel, uint8_t direction, uint8_t layer);
    Packet(uint8_t* data_, uint16_t size_, uint8_t event, uint8_t direction_, uint8_t layer_);
    Packet(const std::vector<uint8_t>& data, CHANNEL channel, uint8_t direction, uint8_t layer);
    Packet(const Packet& p);
    Packet(Packet *p);

    Packet& operator=(const Packet& p);
    Packet& operator=(Packet* p);

    bool operator==(const Packet& p) const;

public:
    inline uint32_t get_size() const {
        return data_.size();
    }

    inline CHANNEL get_channel() const {
        return channel_;
    }

    inline uint8_t* get_raw_data() {
        return data_.data();
    }

    inline std::vector<uint8_t>& get_data_ref() {
        return data_;
    }

    inline const std::vector<uint8_t>& get_data() const {
        return data_;
    }

    inline uint8_t get_direction() const {
        return direction_;
    }

    inline const std::string& get_dissector_name() const {
        return dissector_name_;
    }

    inline uint8_t get_layer() const {
        return layer_;
    }

    inline uint8_t get_event() const {
        return event_;
    }

    inline const std::string& get_summary() const {
        return packet_summary_;
    }

    inline const std::string& get_summary_short() const {
        return packet_summary_short_;
    }

    inline const std::vector<Field>& get_fields() const {
        return fields_;
    }

public:
    inline void set_dissector_name(const std::string& dissector_name) {
        dissector_name_ = dissector_name;
    }

public:
    friend struct PacketHash;
    friend void to_json(nlohmann::json& j, const Packet& packet);
    friend void from_json(const nlohmann::json& j, Packet& packet);

    void init_fields();
    std::ostream& dump(std::ostream& os) const;
    std::ostream& dump_fuzzed_packet(std::ostream&, std::vector<parsed_field*>& fuzzed_fields) const; // Print packet highlighting the fuzzed bytes
    void set_initial_field_mutation_probabilities();

    void set_packet_summary(const std::string& packet_summary);

    /* Quick dissect: gets information about the packet summary (name, protocol, etc.). Does not dissect any of the fields! */
    bool quick_dissect();
    /* Full dissect: performs quick_dissect and on top of that initializes all the fields in the packet (and assignes the mutation probabilities to them)*/
    bool full_dissect();

private:
    std::vector<uint8_t> data_;
    std::vector<Field> fields_;
    CHANNEL channel_;
    uint8_t layer_;
    uint8_t event_;
    uint8_t direction_;
    std::string dissector_name_;
    
    std::string packet_summary_short_ = "";
    std::string packet_summary_ = "";

public:
    static uint8_t test_navigate_callback(proto_tree *subnode, uint8_t field_type, uint8_t *pkt_buf);
    static bool parse_field(field_info *field, parsed_field *f);
    static std::vector<std::string> expected_prefixes;
    static PROTOCOL_TYPE protocol_type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Packet, data_, direction_, packet_summary_, packet_summary_short_)
};

std::ostream& operator<<(std::ostream& os, const Packet& packet);

struct PacketHash {
    size_t operator()(const Packet& packet) const {
        std::hash<uint8_t> hasher;
        size_t answer = 0;
        for (uint8_t i : packet.get_data()) {
            answer ^= hasher(i) + 0x9e3779b9 + (answer << 6) + (answer >> 2);
        }
        answer ^= std::hash<std::string>()(packet.get_summary_short());
        return answer;
    }
};
