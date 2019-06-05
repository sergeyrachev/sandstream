#pragma once

#include <cstddef>
#include <cstdint>

#include "masked_two_bytes_value.h"
#include "pmt.h"

namespace details{
    template<typename Storage>
    size_t parse_pmt_payload( size_t payload_size, const Storage &storage, size_t position, challenge::pmt_t& tracks){
        using masked_two_bytes_value_t = challenge::masked_two_bytes_value_tt<Storage>;

        size_t payload_position = 0;

        static const size_t ignored_reserved_and_pcr_pid_fields_length = 2;
        payload_position += ignored_reserved_and_pcr_pid_fields_length;

        static const uint32_t program_info_length_msb_mask = 0x0f;
        uint32_t program_info_length = masked_two_bytes_value_t(storage, payload_position + position, program_info_length_msb_mask).value;
        payload_position += masked_two_bytes_value_t::two_byte_value_length;
        payload_position += program_info_length;

        static const size_t crc_length = 4;
        while (payload_position < payload_size - crc_length) {

            uint8_t stream_type = storage[payload_position + position];
            static const size_t stream_type_presence = 1;
            payload_position += stream_type_presence;

            static const uint32_t elementary_pid_msb_mask = 0x1f;
            uint16_t elementary_pid = masked_two_bytes_value_t(storage, payload_position + position, elementary_pid_msb_mask).value;
            payload_position += masked_two_bytes_value_t::two_byte_value_length;

            static const uint32_t es_info_length_msb_mask = 0x0f;
            uint16_t es_info_length = masked_two_bytes_value_t(storage, payload_position + position, es_info_length_msb_mask).value;
            payload_position += masked_two_bytes_value_t::two_byte_value_length;
            payload_position += es_info_length;

            // PSI updates are not supported
            tracks.emplace(elementary_pid, stream_type);
        }
        payload_position += crc_length;

        return payload_position + position;
    }
}
