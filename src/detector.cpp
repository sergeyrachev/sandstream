#include "detector.h"

#include "ts_packet.h"

#include <cassert>

size_t challenge::detector_t::detect(const uint8_t *data, size_t available_bytes) {
    size_t consumed_bytes = 0;
    consumed_bytes += ts_packet_t::sync(data, available_bytes);

    if(available_bytes - consumed_bytes < ts_packet_t::ts_packet_size){
        return 0;
    }

    ts_packet_t packet(data + consumed_bytes, available_bytes - consumed_bytes);

    static const uint32_t pid_pat = 0;
    if(packet.payload_unit_start_indicator){
        if(packet.pid == pid_pat){
            parse_pat(packet.payload + 1, packet.payload_size - 1);
        } else if(packet.pid == pmt_pid){
            parse_pmt(packet.payload + 1, packet.payload_size - 1);
        }
    }
    consumed_bytes += ts_packet_t::ts_packet_size;
    return consumed_bytes;
}

void challenge::detector_t::parse_pat(const uint8_t *data, size_t available_bytes) {
    static const size_t section_header_length = 3;
    static const size_t crc_length = 4;
    size_t consumed_bytes = 0;
    consumed_bytes += 1;

    uint32_t section_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
    consumed_bytes += 2;

    assert(section_length + section_header_length <= available_bytes);

    consumed_bytes += 5;
    while(consumed_bytes < section_length + section_header_length - crc_length){

        consumed_bytes += 2;

        pmt_pid = (data[consumed_bytes] & 0x1Ful) << 8ul | data[consumed_bytes + 1];
        consumed_bytes += 2;
    }
}

void challenge::detector_t::parse_pmt(const uint8_t *data, size_t available_bytes) {

    elementary_stream_pids.clear();

    static const size_t section_header_length = 3;
    static const size_t crc_length = 4;
    size_t consumed_bytes = 0;
    consumed_bytes += 1;

    uint32_t section_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
    consumed_bytes += 2;

    assert(section_length + section_header_length <= available_bytes);
    consumed_bytes += 7;

    uint32_t program_info_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
    consumed_bytes += 2;
    consumed_bytes += program_info_length;

    while(consumed_bytes < section_length + section_header_length - crc_length){

        consumed_bytes += 1;

        uint32_t elementary_pid = (data[consumed_bytes] & 0x1Ful)<< 8ul | data[consumed_bytes + 1];
        consumed_bytes += 2;

        uint32_t es_info_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
        consumed_bytes += 2;
        consumed_bytes += es_info_length;

        elementary_stream_pids.push_back(elementary_pid);
    }
}
