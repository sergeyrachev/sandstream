#include "ts_packet.h"

#include "masked_two_bytes_value.h"

#include <cassert>

challenge::ts_packet_t::ts_packet_t(const uint8_t *data, size_t available_bytes) {
    using masked_two_bytes_value_t = masked_two_bytes_value_tt<const uint8_t *>;
    size_t position = 0;
    assert(data);
    assert(data[position] == ts_packet_t::ts_sync_byte);
    assert(available_bytes == ts_packet_t::ts_packet_size);
    position++;

    static const uint32_t payload_unit_start_indicator_mask = 0x40;
    payload_unit_start_indicator = data[position] & payload_unit_start_indicator_mask;

    static const uint32_t pid_msb_mask = 0x1f;
    pid = masked_two_bytes_value_t(data, position, pid_msb_mask).value;
    position += masked_two_bytes_value_t::two_byte_value_length;

    static const uint32_t adaptation_field_control_mask = 0x30;
    static const uint32_t adaptation_field_control_shift = 4;

    uint8_t adaptation_field_control = ((data[position++] & adaptation_field_control_mask) >> adaptation_field_control_shift);

    const uint8_t* payload_data = data + position;
    size_t payload_size = ts_packet_t::ts_packet_size - position;

    static const uint8_t payload_only = 0x01;
    static const uint8_t adaptation_field_control_only = 0x02;
    static const uint8_t adaptation_field_control_and_payload = 0x03;

    if(adaptation_field_control_and_payload == adaptation_field_control || adaptation_field_control_only == adaptation_field_control) {
        uint8_t adaptation_field_length = payload_data[0];
        static const size_t adaptation_field_length_presence = 1;

        const size_t adaptation_field_skip_size = adaptation_field_length_presence + adaptation_field_length;
        payload_data += adaptation_field_skip_size;
        payload_size -= adaptation_field_skip_size;
    }
    payload.assign(payload_data, payload_data + payload_size);
}

challenge::ts_packet_t::ts_packet_t(uint16_t pid, bool payload_unit_start_indicator, const std::vector<uint8_t>& payload)
: pid(pid)
, payload_unit_start_indicator(payload_unit_start_indicator)
, payload(payload){

}
