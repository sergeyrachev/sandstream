#include "pes_packet.h"

#include <cassert>
#include <algorithm>

challenge::pes_packet_t::pes_packet_t(const uint8_t *data, size_t available_bytes) {
    payload = nullptr;
    payload_size = 0;

    if(!parse_header(data, available_bytes)){
        storage.assign(data, data + available_bytes);
    }
}

void challenge::pes_packet_t::assign(const uint8_t *data, size_t available_bytes) {
    if(!storage.empty()){
        std::copy(data, data + available_bytes, std::back_inserter(storage));
        if(parse_header(storage.data(), storage.size())){
            storage.clear();
        }
    } else {
        payload = data;
        payload_size = available_bytes;
    }

    if(pes_packet_length){
        payload_size = std::min(pes_packet_length, available_bytes);
        pes_packet_length -= payload_size;
    }
}

bool challenge::pes_packet_t::parse_header(const uint8_t *data, size_t available_bytes) {
    payload = nullptr;
    payload_size = 0;

    if(available_bytes < packet_start_code_prefix_length + PES_packet_length_length + stream_id_length){
        return false;
    }

    assert(data[0] == 0x00);
    assert(data[1] == 0x00);
    assert(data[2] == 0x01);

    stream_id = data[3];
    pes_packet_length = uint32_t(data[4] << 8ul) | uint32_t(data[5]);

    static const uint8_t program_stream_map = 0xbc;
    static const uint8_t padding_stream = 0xbe;
    static const uint8_t private_stream_2 = 0xbf;
    static const uint8_t ecm_stream = 0xf0;
    static const uint8_t emm_stream = 0xf1;
    static const uint8_t dsmcc_stream = 0xf2;
    static const uint8_t h222_type_e_stream = 0xf8;
    static const uint8_t program_stream_directory = 0xFF;

    if( stream_id != program_stream_map &&
        stream_id != padding_stream &&
        stream_id != private_stream_2 &&
        stream_id != ecm_stream &&
        stream_id != emm_stream &&
        stream_id != program_stream_directory &&
        stream_id != dsmcc_stream &&
        stream_id != h222_type_e_stream ){

        if(available_bytes < 9){
            return false;
        }

        uint8_t pes_header_data_length = data[8];
        static const uint8_t flags_length = 3;
        size_t consumed_bytes = flags_length;

        if(available_bytes <  packet_start_code_prefix_length + PES_packet_length_length + stream_id_length + consumed_bytes + pes_header_data_length){
            return false;
        }

        consumed_bytes += pes_header_data_length;

        size_t payload_offset = packet_start_code_prefix_length + PES_packet_length_length + stream_id_length + consumed_bytes;

        payload = data + payload_offset;
        payload_size = available_bytes - payload_offset;

        if(pes_packet_length){
            pes_packet_length -= (pes_header_data_length + flags_length);
        }

    } else if( stream_id == program_stream_map ||
               stream_id == private_stream_2 ||
               stream_id == ecm_stream ||
               stream_id == emm_stream ||
               stream_id == program_stream_directory ||
               stream_id == dsmcc_stream ||
               stream_id == h222_type_e_stream ){

        payload = data;
        payload_size = available_bytes;

    } else if(stream_id == padding_stream){

        payload = data;
        payload_size = available_bytes;
    }

    return true;
}
