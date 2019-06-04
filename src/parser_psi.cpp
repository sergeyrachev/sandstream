#include "parser_psi.h"

#include "masked_two_bytes_value.h"

void challenge::parser_psi_t::put(std::unique_ptr<ts_packet_t> packet) {
    std::shared_ptr <ts_packet_t> shared_packet(std::move(packet));
    if (shared_packet->payload_unit_start_indicator) {
        uint8_t pointer_field = shared_packet->payload[0];
        static const size_t pointer_field_presence = 1;
        storage.push_back({shared_packet, shared_packet->payload.data() + pointer_field_presence, pointer_field});
        storage.push_back({shared_packet,
                          shared_packet->payload.data() + pointer_field_presence + pointer_field,
                          shared_packet->payload.size() - pointer_field_presence - pointer_field});
    } else {
        storage.push_back({shared_packet, shared_packet->payload.data(), shared_packet->payload.size()});
    }

    size_t consumed_bytes = 0;
    // I would rather prefer std::get<bool>, std::get<size_t> and so on but it is supported since c++14 only.
    static const uint8_t bool_index = 0;
    for (auto parse_header_result = parser_psi_t::try_parse(storage, consumed_bytes); std::get<bool_index>(parse_header_result); parse_header_result = parser_psi_t::try_parse(storage, consumed_bytes)) {

        static const uint8_t section_header_t_index = 1;
        section_header_t header = std::get<section_header_t_index>(parse_header_result);;

        const size_t payload_size = header.section_length - section_header_t::section_ext_header_length;

        consumed_bytes = parse_payload(header, payload_size, storage, consumed_bytes);
        consumed_bytes = skip_padding_bytes(storage, consumed_bytes);

        if (header.section_number == header.last_section_number) {
            update();
        }
    }

    storage.pop_front(consumed_bytes);
}

size_t challenge::parser_psi_t::parse_payload(const challenge::section_header_t &header, size_t payload_size, const storage_t &storage, size_t position) {
    return payload_size + position;
}

void challenge::parser_psi_t::update() {
    // Not implemented
}

size_t challenge::parser_psi_t::skip_padding_bytes(const storage_t &storage, size_t position) {
    static const uint8_t section_padding_byte = 0xff;
    auto storaged_size = storage.size();
    while (position < storaged_size && storage[position] == section_padding_byte) {
        position++;
    }
    return position;
}

std::tuple<bool, challenge::section_header_t> challenge::parser_psi_t::try_parse(const storage_t &storage, size_t &consumed_bytes) {
    using challenge::masked_two_bytes_value_t;

    auto storaged_size = storage.size();
    if (storaged_size - consumed_bytes < section_header_t::section_header_length) {
        return {};
    }

    uint8_t table_id = storage[consumed_bytes++];

    static const uint32_t section_length_msb_mask = 0x0f;
    uint32_t section_length = masked_two_bytes_value_t<storage_t>(storage, consumed_bytes, section_length_msb_mask).value;
    consumed_bytes += masked_two_bytes_value_t<storage_t>::two_byte_value_length;

    if (storaged_size - consumed_bytes < section_length) {
        return {};
    }

    static const size_t transport_stream_id_length = 2;
    static const size_t ignored_fields_length = 1;
    consumed_bytes += transport_stream_id_length + ignored_fields_length;

    uint8_t section_number = storage[consumed_bytes++];
    uint8_t last_section_number = storage[consumed_bytes++];

    section_header_t header{};

    header.section_length = section_length;
    header.section_number = section_number;
    header.last_section_number = last_section_number;

    return {true, header};
}
