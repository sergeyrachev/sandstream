#include "packetizer.h"

#include <algorithm>
#include <cassert>

using challenge::ts_packet_t;



challenge::packetizer_t::packetizer_t(size_t max_buffer_size) : data_storage(max_buffer_size), input_buffer{data_storage.data(), 0} {

}

challenge::mutable_buffer_t challenge::packetizer_t::request() {
    mutable_buffer_t requested_buffer{data_storage.data() + input_buffer.size, data_storage.size() - input_buffer.size};
    return requested_buffer;
}

void challenge::packetizer_t::confirm(challenge::mutable_buffer_t filled_buffer) {
    size_t position = 0;
    size_t available_bytes = input_buffer.size + filled_buffer.size;
    while(available_bytes - position >= ts_packet_t::ts_packet_size){
        size_t consumed = 1;
        if(input_buffer.data[position] == ts_packet_t::ts_sync_byte) {
            packet_sequence.emplace_back(input_buffer.data + position, ts_packet_t::ts_packet_size);
            consumed = ts_packet_t::ts_packet_size;
        }
        position += consumed;
    }

    size_t tail_size = available_bytes - position;
    memmove(data_storage.data(), data_storage.data() + position, tail_size);
    input_buffer.size = tail_size;
}

bool challenge::packetizer_t::is_depleted() const {
    return packet_sequence.empty();
}

ts_packet_t challenge::packetizer_t::get() {
    auto packet = packet_sequence.front();
    packet_sequence.pop_front();
    return packet;
}
