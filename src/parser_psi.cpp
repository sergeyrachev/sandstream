#include "parser_psi.h"

#include "masked_two_bytes_value.h"

void challenge::parser_psi_t::put(challenge::ts_packet_t packet) {
    auto sections_payload = packet.payload.data;
    auto sections_size = packet.payload.size;

    if (packet.payload_unit_start_indicator) {
        static const size_t pointer_field_presence = 1;
        sections_payload += pointer_field_presence;
        sections_size -= pointer_field_presence;
    }
    section_data.insert(section_data.end(), sections_payload, sections_payload + sections_size);

    size_t consumed_bytes = 0;
    auto parse_result = parser_psi_t::try_parse_header(section_data.data() + consumed_bytes, section_data.size() - consumed_bytes);
    while(parse_result.is_success){
        consumed_bytes += parse_result.consumed_bytes;

        const section_header_t& header = parse_result.header;

        static const size_t section_ext_header_length = 5;
        const size_t payload_size = header.section_length - section_ext_header_length;

        on_payload(section_data.data() + consumed_bytes, payload_size );

        consumed_bytes += payload_size;
        consumed_bytes += skip_padding_bytes(section_data.data() + consumed_bytes, section_data.size() - consumed_bytes);

        parse_result = parser_psi_t::try_parse_header(section_data.data() + consumed_bytes, section_data.size() - consumed_bytes);
    }

    if(consumed_bytes){
        section_data.erase(section_data.begin(), section_data.begin() + consumed_bytes);
    }
}

size_t challenge::parser_psi_t::skip_padding_bytes(const uint8_t* data, size_t available) {
    static const uint8_t section_padding_byte = 0xff;

    size_t position = 0;
    while (position < available && data[position] == section_padding_byte) {
        position++;
    }
    return position;
}

challenge::parser_psi_t::parse_result_t challenge::parser_psi_t::try_parse_header(const uint8_t* data, size_t available) {

    static const size_t section_header_length = 3;

    if (available < section_header_length) {
        return {};
    }

    size_t position = 0;

    static const size_t table_id_presence = 1;
    position += table_id_presence;

    static const uint32_t section_length_msb_mask = 0x0f;
    uint16_t section_length = masked_two_bytes_value_t(data + position, section_length_msb_mask).value;
    position += masked_two_bytes_value_t::two_byte_value_length;

    if (available - position < section_length) {
        return {};
    }

    static const size_t transport_stream_id_length = 2;
    static const size_t ignored_fields_length = 1;
    position += transport_stream_id_length + ignored_fields_length;

    static const size_t section_number_presence = 1;
    position += section_number_presence;

    static const size_t last_section_number_presence = 1;
    position += last_section_number_presence;

    section_header_t header{};
    header.section_length = section_length;

    return {true, position, header};
}
