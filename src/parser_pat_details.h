#pragma once

#include <cstddef>
#include <cstdint>

#include "masked_two_bytes_value.h"
#include "pat.h"

namespace details{

    template<typename Storage>
    size_t parse_pat_payload( size_t payload_size, const Storage &storage, size_t position, challenge::pat_t& services){
        using masked_two_bytes_value_t = challenge::masked_two_bytes_value_tt<Storage>;
        using pat_t = challenge::pat_t;

        static const size_t crc_length = 4;
        size_t payload_position = 0;
        while (payload_position < payload_size - crc_length) {
            uint16_t program_number = masked_two_bytes_value_t(storage, payload_position + position).value;
            payload_position += masked_two_bytes_value_t::two_byte_value_length;

            static const uint32_t pid_msb_mask = 0x1f;
            uint16_t pid = masked_two_bytes_value_t(storage, payload_position + position, pid_msb_mask).value;
            payload_position += masked_two_bytes_value_t::two_byte_value_length;

            services.emplace(pid, program_number);
        }
        payload_position += crc_length;
        return payload_position + position;
    }
}
