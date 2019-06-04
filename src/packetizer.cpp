#include "packetizer.h"

#include <algorithm>
#include <cassert>

using challenge::ts_packet_t;

std::unique_ptr<ts_packet_t> challenge::packetizer_t::get() {
    if(sequence.empty()){
        return nullptr;
    }

    auto packet = std::move(sequence.front());
    sequence.pop_front();
    return packet;
}

void challenge::packetizer_t::put(const uint8_t *data, size_t size) {

    auto skip_bytes = recover_partial_packet(data, size);

    while(size - skip_bytes > 0){
        if(data[skip_bytes] == ts_packet_t::ts_sync_byte){
            if(size - skip_bytes >= ts_packet_t::ts_packet_size){
                sequence.push_back(std::unique_ptr<ts_packet_t>{new ts_packet_t(data + skip_bytes, ts_packet_t::ts_packet_size)});
                skip_bytes += ts_packet_t::ts_packet_size;
            } else {
                store_partial_packet(data + skip_bytes, size - skip_bytes);
                break;
            }
        } else {
            skip_bytes++;
        }
    }
}

size_t challenge::packetizer_t::recover_partial_packet(const uint8_t *data, size_t size) {
    if(partial_packet.empty()){
        return 0;
    }

    auto tail_size = std::min(size, ts_packet_t::ts_packet_size - partial_packet.size());
    partial_packet.insert(partial_packet.end(), data, data + tail_size);

    // Stream discontinuity may happen between calls; check sync
    bool has_packet = partial_packet.size() == ts_packet_t::ts_packet_size && partial_packet.front() == ts_packet_t::ts_sync_byte;
    bool has_sync = (size - tail_size) == 0 || data[tail_size] == ts_packet_t::ts_sync_byte;
    if( has_packet && has_sync){
        sequence.push_back(std::unique_ptr<ts_packet_t>{new ts_packet_t(partial_packet.data(), partial_packet.size())});
        partial_packet.clear();
    }
    return tail_size;
}

void challenge::packetizer_t::store_partial_packet(const uint8_t *data, size_t size) {
    std::copy_n(data, size, std::back_inserter(partial_packet));
}
