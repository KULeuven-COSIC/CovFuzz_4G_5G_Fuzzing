#include "packet.h"

#include "helpers.h"
#include "my_logger.h"

#include "Configs/Fuzzing_Strategies/fuzz_strategy_config.h"
#include "Fuzzers/base_fuzzer.h"

#include "dissector.h"

#include <string>
#include <unordered_map>

extern Fuzz_Strategy_Config fuzz_strategy_config_g;
extern My_Logger my_logger_g;

static std::vector<Field> g_fields;
static std::unordered_map<std::string, int> field_name_counter;

std::vector<std::string> Packet::expected_prefixes;
PROTOCOL_TYPE Packet::protocol_type;

Packet::Packet() {};

Packet::Packet (uint8_t* data, uint16_t size, CHANNEL channel, uint8_t direction, uint8_t layer)  {
    data_.reserve(size);
    data_ = std::vector<uint8_t>(data, data + size);
    channel_ = channel;
    event_ = 0;
    direction_ = direction;
    layer_ = layer;
}

Packet::Packet (uint8_t* data, uint16_t size, uint8_t event, uint8_t direction, uint8_t layer)  {
    data_.reserve(size);
    data_ = std::vector<uint8_t>(data, data + size);
    event_ = event;
    channel_ = CHANNEL::UNKNOWN;
    direction_ = direction;
    layer_ = layer;
}

Packet::Packet (const std::vector<uint8_t>& data, CHANNEL channel, uint8_t direction, uint8_t layer) 
    : data_(data), channel_(channel), layer_(layer), direction_(direction) {}

Packet::Packet(const Packet& p) {
    data_ = p.data_;
    channel_ = p.channel_;
    event_ = p.event_;
    fields_ = p.fields_;
    direction_ = p.direction_;
    layer_ = p.layer_;
    packet_summary_ = p.packet_summary_;
    packet_summary_short_ = p.packet_summary_short_;
    dissector_name_ = p.dissector_name_;
}

Packet::Packet(Packet *p) {
    data_ = p->data_;
    channel_ = p->channel_;
    event_ = p->event_;
    fields_ = p->fields_;
    direction_ = p->direction_;
    layer_ = p->layer_;
    packet_summary_ = p->packet_summary_;
    packet_summary_short_ = p->packet_summary_short_;
    dissector_name_ = p->dissector_name_;
}

Packet& Packet::operator=(const Packet& p) {
    data_ = p.data_;
    channel_ = p.channel_;
    event_ = p.event_;
    fields_ = p.fields_;
    direction_ = p.direction_;
    layer_ = p.layer_;
    packet_summary_ = p.packet_summary_;
    packet_summary_short_ = p.packet_summary_short_;
    dissector_name_ = p.dissector_name_;
    return *this;
}

Packet& Packet::operator=(Packet* p) {
    data_ = p->data_;
    channel_ = p->channel_;
    event_ = p->event_;
    fields_ = p->fields_;
    direction_ = p->direction_;
    layer_ = p->layer_;
    packet_summary_ = p->packet_summary_;
    packet_summary_short_ = p->packet_summary_short_;
    dissector_name_ = p->dissector_name_;
    return *this;
}

bool Packet::operator==(const Packet &p) const
{
    return data_ == p.data_ && packet_summary_short_ == p.packet_summary_short_ && direction_ == p.direction_;
}

void Packet::set_initial_field_mutation_probabilities() {
    double mut_prob = 1.0 * fuzz_strategy_config_g.init_prob_mult_factor / fields_.size();
    my_logger_g.logger->debug("Setting probs to value {}", mut_prob);
    for (auto& field : fields_) {
        field.set_mutation_probability(mut_prob);
    }
}

void Packet::set_packet_summary(const std::string &packet_summary_)
{
    this->packet_summary_ = packet_summary_;
    packet_summary_short_ = helpers::shorten_dissector_summary(packet_summary_);
}

bool Packet::quick_dissect() {
    if (!Dissector::set_dissector(dissector_name_)) {
        my_logger_g.logger->warn("Failed to set a dissector: {}", dissector_name_);
        return false;
    } 
    
    if (!Dissector::dissect_packet(data_)) {
        my_logger_g.logger->warn("Could not dissect the packet!");
        return false;
    }

    set_packet_summary(Dissector::get_last_packet_summary());

    return true;
}

bool Packet::full_dissect() {
    if (!quick_dissect()) return false;

    if (auto it = Base_Fuzzer::packet_fields_database.find(packet_summary_short_ + std::to_string(data_.size())); it != Base_Fuzzer::packet_fields_database.end()) {
        fields_ = it->second;
    } else {
        my_logger_g.logger->debug("Initializing packet: {}", packet_summary_short_);
        init_fields();
        set_initial_field_mutation_probabilities();
        Base_Fuzzer::packet_fields_database[packet_summary_short_ + std::to_string(data_.size())] = fields_;
    }

    return true;
}

std::ostream& Packet::dump_fuzzed_packet(std::ostream& os, std::vector<parsed_field*>& fuzzed_fields) const {
    for (size_t i = 0; i < get_size(); i++) {
        os << CRESET << std::hex << std::uppercase;
        for (const parsed_field* parsed_field_ptr : fuzzed_fields) {
            if (i >= parsed_field_ptr->offset && i < parsed_field_ptr->offset + parsed_field_ptr->length) {
                os << BLUE;
            }
        }
        os << std::setfill('0') << std::setw(2) << data_.at(i);
    }
    os << CRESET << std::dec << std::nouppercase << std::endl;
    return os;
}

std::ostream& Packet::dump(std::ostream& os) const {
    os << std::hex << std::uppercase;
    for (size_t i = 0; i < get_size(); i++) {
        os << std::setfill('0') << std::setw(2) << static_cast<int>(data_.at(i)) << " ";
    }
    os << std::dec << std::nouppercase;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Packet& packet) {
    return packet.dump(os);
}

uint8_t Packet::test_navigate_callback(proto_tree *subnode, uint8_t field_type, uint8_t *pkt_buf) {
    static bool in_fix_mode = false; // Fix mode allows me to use different aligns to what dissector returns to me

    if (field_type == WD_TYPE_FIELD) {
        const std::string& short_field_name = subnode->finfo->hfinfo->abbrev;
        for (const auto& prefix : Packet::expected_prefixes) {
            if (short_field_name.starts_with(prefix)) {
                //Packet::field_names.insert(short_field_name);
                field_name_counter[short_field_name]++;
                parsed_field pf;
                if (!Packet::parse_field(subnode->finfo, &pf)) {
                    my_logger_g.logger->warn("Counldn't parse field {}", short_field_name);
                    break;
                }
                if (in_fix_mode) {
                    if (pf.mask_align == 2) {
                        pf.mask_align = 3; // QUICK BUG FIX: Align should be 3 bits instead of 2!
                    }
                }
                Field f(short_field_name, pf);
                f.index = field_name_counter[short_field_name] - 1;
                //f.value = packet_read_field_uint64(subnode->finfo) - (uint64_t)subnode->finfo->value_min;
                g_fields.push_back(std::move(f));
                break;
            }
        }
        
    } else if (field_type == WD_TYPE_GROUP) {
        if (!strcmp(subnode->finfo->hfinfo->name, "masterCellGroup") && subnode->finfo->ds_tvb->bitshift_from_octet == 2 && Packet::protocol_type == PROTOCOL_TYPE::PROTO_5G) {        
            in_fix_mode = true;
            my_logger_g.logger->info("Entering the fix mode (protocol type: {})", (int)Packet::protocol_type);
            // TODO: Find the source of the bug in the dissector itself & Fix it
        }
    } else if (field_type == WD_TYPE_LAYER) {
        if (in_fix_mode) {
            in_fix_mode = false; // Quit the fix mode when going to another layer_
            my_logger_g.logger->info("Out of the fix mode");
        }
    }

    return 0;
}
    

void Packet::init_fields()
{
    Packet::expected_prefixes = get_field_prefixes_by_layer_idx(this->layer_, Packet::protocol_type);

    g_fields.clear();
    field_name_counter.clear();

    packet_navigate(1, 0, test_navigate_callback);
    my_logger_g.logger->debug("Inited {} fields", g_fields.size());

    this->fields_ = std::move(g_fields);
}

bool Packet::parse_field(field_info *field, parsed_field *f)
{
    f->offset = packet_read_field_offset(field);
    f->mask = packet_read_field_bitmask(field);
    if (f->mask == 0) {
        return false;
    }
    f->length = packet_read_field_size(field);

	if (f->mask != 0) {
		uint16_t mask_offset = 0;
		uint64_t cur_mask = f->mask;
		cur_mask >>= 8 * (f->length - 1);
		assert(cur_mask != 0);
		for (size_t i = 0; i < 8; i++) {
			if ((cur_mask >> i) == 0) {
				mask_offset++;
			} else {
				mask_offset = 0;
			}
		}
		f->mask_offset = mask_offset;
	}

    f->mask_align = field->ds_tvb->bitshift_from_octet;
    return true;
}
