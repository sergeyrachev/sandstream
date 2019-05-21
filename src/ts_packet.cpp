#include "ts_packet.h"

#include <cassert>

challenge::ts_packet_t::ts_packet_t(const uint8_t *data, size_t available_bytes) {
    assert(*data == ts_sync_byte);
    assert(available_bytes >= ts_packet_size);

    payload_unit_start_indicator = (data[1] & 0x40ul) == 0x40;
    pid = ((data[1] & 0x1Ful) << 8ul) + (data[2] & 0xFFul);
    uint8_t adaptation_field_control = ((data[3] & 0x30ul) >> 4ul);

    adaptation_field = nullptr;
    adaptation_field_size = 0;

    payload = data + 4;
    payload_size = ts_packet_size - 4;

    static const uint8_t adaptation_field_only = 0x02;
    static const uint8_t adaptation_field_and_payload = 0x03;

    if(adaptation_field_and_payload == adaptation_field_control || adaptation_field_only == adaptation_field_control) {
        uint8_t adaptation_field_length = payload[0];

        payload += 1;
        payload_size -=1;

        adaptation_field = payload;
        adaptation_field_size = adaptation_field_length;

        payload += adaptation_field_length;
        payload_size -= adaptation_field_length;
    }
}

size_t challenge::ts_packet_t::sync(const uint8_t *data, size_t available_bytes) {
    size_t skiped_bytes = 0;
    while( skiped_bytes < available_bytes && *(data + skiped_bytes) != ts_sync_byte){
        skiped_bytes++;
    }
    return skiped_bytes;
}
