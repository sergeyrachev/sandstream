#include "parser_pes.h"

#include "masked_two_bytes_value.h"

#include <cassert>

challenge::parser_pes_t::parser_pes_t(std::unique_ptr<callback_es_t> consumer) : consumer(std::move(consumer)) {

}

void challenge::parser_pes_t::put(challenge::ts_packet_t packet) {

    auto payload_data = packet.payload.data;
    auto payload_size = packet.payload.size;

    if (packet.payload_unit_start_indicator) {
        auto parse_result = try_parse_header(payload_data, payload_size);
        if( parse_result.is_success ){
            payload_data += parse_result.consumed_bytes;
            payload_size -= parse_result.consumed_bytes;
        }
    }
    if(payload_size){
        consumer->on_data({payload_data, payload_size});
    }
}

challenge::parser_pes_t::parse_result_t challenge::parser_pes_t::try_parse_header(const uint8_t* data, size_t available){
    using challenge::masked_two_bytes_value_t;
    static const size_t packet_start_code_prefix_length = 3;
    static const size_t stream_id_length = 1;

    static const size_t pes_packet_length_length = masked_two_bytes_value_t::two_byte_value_length;

    if (available < packet_start_code_prefix_length + pes_packet_length_length + stream_id_length) {
        return {};
    }

    size_t position = 0;
    position += packet_start_code_prefix_length;

    uint8_t stream_id = data[position];
    position += stream_id_length;

    uint16_t pes_packet_length = masked_two_bytes_value_t(data + position).value;
    position += pes_packet_length_length;

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
        if (available - position < pes_header_data_length_offset) {
            return {};
        }
        position += pes_header_data_length_offset;

        uint8_t pes_header_data_length = data[position++];
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

    return {true, position, };
}
