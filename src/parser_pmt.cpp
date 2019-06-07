#include "parser_pmt.h"

#include "masked_two_bytes_value.h"

challenge::parser_pmt_t::parser_pmt_t(challenge::callback_pmt_t &callback) : callback(callback) {

}

void challenge::parser_pmt_t::on_payload(const uint8_t* data, size_t available) {
    callback.on_pmt(parse_pmt_payload(data, available));
}

challenge::pmt_t challenge::parser_pmt_t::parse_pmt_payload(const uint8_t *data, size_t payload_size) {
    using challenge::masked_two_bytes_value_t;

    size_t position = 0;

    static const size_t ignored_reserved_and_pcr_pid_fields_length = 2;
    position += ignored_reserved_and_pcr_pid_fields_length;

    static const uint32_t program_info_length_msb_mask = 0x0f;
    uint32_t program_info_length = masked_two_bytes_value_t(data + position, program_info_length_msb_mask).value;
    position += masked_two_bytes_value_t::two_byte_value_length;

    position += program_info_length;

    pmt_t pmt;
    static const size_t crc_length = 4;
    while (position < payload_size - crc_length) {

        static const size_t stream_type_presence = 1;
        uint8_t stream_type = data[position];
        position += stream_type_presence;

        static const uint32_t elementary_pid_msb_mask = 0x1f;
        uint16_t elementary_pid = masked_two_bytes_value_t(data + position, elementary_pid_msb_mask).value;
        position += masked_two_bytes_value_t::two_byte_value_length;

        static const uint32_t es_info_length_msb_mask = 0x0f;
        uint16_t es_info_length = masked_two_bytes_value_t(data + position, es_info_length_msb_mask).value;
        position += masked_two_bytes_value_t::two_byte_value_length;
        position += es_info_length;

        pmt.emplace(elementary_pid, stream_type);
    }

    return pmt;
}
