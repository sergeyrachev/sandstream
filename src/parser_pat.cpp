#include "parser_pat.h"

#include "masked_two_bytes_value.h"

challenge::parser_pat_t::parser_pat_t(challenge::callback_pat_t &handler) : callback(handler) {}

void challenge::parser_pat_t::on_payload(const uint8_t* data, size_t available) {
    callback.on_pat(parse_pat_payload(data, available));
}

challenge::pat_t challenge::parser_pat_t::parse_pat_payload(const uint8_t *data, size_t available) {
    using challenge::masked_two_bytes_value_t;

    static const size_t crc_length = 4;
    pat_t pat;
    size_t position = 0;
    while (position < available - crc_length) {
        uint16_t program_number = masked_two_bytes_value_t(data + position).value;
        position += masked_two_bytes_value_t::two_byte_value_length;

        static const uint32_t pid_msb_mask = 0x1f;
        uint16_t pid = masked_two_bytes_value_t(data + position, pid_msb_mask).value;
        position += masked_two_bytes_value_t::two_byte_value_length;

        pat.emplace(pid, program_number);
    }
    position += crc_length;
    return pat;
}
