#include "parser_pes.h"

#include "masked_two_bytes_value.h"

#include <cassert>

challenge::parser_pes_t::parser_pes_t(std::unique_ptr<callback_es_t> consumer) : consumer(std::move(consumer)) {

}

void challenge::parser_pes_t::put(std::unique_ptr<ts_packet_t> packet) {
    std::shared_ptr <ts_packet_t> shared_packet(std::move(packet));
    storage.push_back({shared_packet, shared_packet->payload.data(), shared_packet->payload.size()});

    if (storage.front().packet->payload_unit_start_indicator) {
        size_t consumed_bytes = 0;
        if (try_parse(storage, consumed_bytes)) {
            storage.pop_front(consumed_bytes);
        } else {
            return;
        }
    }

    while (storage.size() != 0) {
        auto entry = storage.front();
        consumer->on_data(entry.data, entry.size);
        storage.pop_front(entry.size);
    }
}

bool challenge::parser_pes_t::try_parse(const storage_t &storage, size_t &position) {
    using masked_two_bytes_value_t = masked_two_bytes_value_t<storage_t>;

    static const uint8_t packet_start_code_prefix_length = 3;
    static const uint8_t stream_id_length = 1;
    static const uint8_t pes_packet_length_length = 2;

    size_t storaged_size = storage.size();
    if (storaged_size - position < packet_start_code_prefix_length + pes_packet_length_length + stream_id_length) {
        return false;
    }
    position += packet_start_code_prefix_length;

    uint8_t stream_id = storage[position++];
    uint16_t pes_packet_length = masked_two_bytes_value_t(storage, position).value;
    position += masked_two_bytes_value_t::two_byte_value_length;

    static const uint8_t program_stream_map = 0xbc;
    static const uint8_t padding_stream = 0xbe;
    static const uint8_t private_stream_2 = 0xbf;
    static const uint8_t ecm_stream = 0xf0;
    static const uint8_t emm_stream = 0xf1;
    static const uint8_t dsmcc_stream = 0xf2;
    static const uint8_t h222_type_e_stream = 0xf8;
    static const uint8_t program_stream_directory = 0xFF;

    if (stream_id != program_stream_map &&
        stream_id != padding_stream &&
        stream_id != private_stream_2 &&
        stream_id != ecm_stream &&
        stream_id != emm_stream &&
        stream_id != program_stream_directory &&
        stream_id != dsmcc_stream &&
        stream_id != h222_type_e_stream) {

        static const size_t pes_header_data_length_offset = 2;
        if (storaged_size - position < pes_header_data_length_offset) {
            return false;
        }
        position += pes_header_data_length_offset;

        uint8_t pes_header_data_length = storage[position++];
        position += pes_header_data_length;

    } else if (stream_id == program_stream_map ||
               stream_id == private_stream_2 ||
               stream_id == ecm_stream ||
               stream_id == emm_stream ||
               stream_id == program_stream_directory ||
               stream_id == dsmcc_stream ||
               stream_id == h222_type_e_stream) {
        // Not implemented
        assert(false);
    } else {
        // Not implemented
        assert(false);
    }

    return true;
}
