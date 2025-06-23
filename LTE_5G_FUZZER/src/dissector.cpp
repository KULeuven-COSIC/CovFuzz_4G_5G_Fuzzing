#include "dissector.h"

#include "wdissector.h"

#include <string>
#include <stdexcept>

namespace Dissector {
    
std::string get_last_packet_protocol()
{
    return packet_protocol();
}

std::string get_last_packet_summary()
{
    return packet_summary();
}

bool set_dissector(const std::string &dissector_name)
{
    if (!packet_set_protocol(("proto:" + dissector_name).c_str())) {
        return false;
    }
    return true;
}

bool dissect_packet(std::vector<uint8_t>& packet) {
    packet_dissect(packet.data(), packet.size());
    /* Check if the dissection was successful */
    if (get_last_packet_summary().empty()) return false;
    return true;
}

}