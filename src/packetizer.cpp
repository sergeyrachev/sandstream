#include "packetizer.h"

#include <algorithm>
#include <cassert>

challenge::packetizer_t::packetizer_t(size_t max_buffer_size) : data_storage(max_buffer_size), input_buffer{data_storage.data(), 0} {

}

challenge::mutable_buffer_t challenge::packetizer_t::request() {
    mutable_buffer_t requested_buffer{data_storage.data() + input_buffer.size, data_storage.size() - input_buffer.size};
    return requested_buffer;
}

void challenge::packetizer_t::confirm(challenge::mutable_buffer_t filled_buffer) {
    size_t available_bytes = input_buffer.size + filled_buffer.size;
    input_buffer.size = available_bytes;
    sync();
}

bool challenge::packetizer_t::is_depleted() const {
    return input_buffer.size < ts_packet_t::ts_packet_size;
}

challenge::ts_packet_t challenge::packetizer_t::get() {
    using challenge::ts_packet_t;

    ts_packet_t packet(input_buffer.data, ts_packet_t::ts_packet_size);
    input_buffer.size -= ts_packet_t::ts_packet_size;
    input_buffer.data += ts_packet_t::ts_packet_size;

    sync();
    return packet;
}

void challenge::packetizer_t::sync() {

    while(input_buffer.size && *input_buffer.data != ts_packet_t::ts_sync_byte ){
        input_buffer.size--;
        input_buffer.data++;
    }

    if(is_depleted()){
        memmove(data_storage.data(), input_buffer.data,  input_buffer.size);
        input_buffer.data = data_storage.data();
    }
}
