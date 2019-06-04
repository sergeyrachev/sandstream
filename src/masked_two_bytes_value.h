#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge {

    template<typename Collection>
    struct masked_two_bytes_value_t {
        static const size_t two_byte_value_length = 2;

        masked_two_bytes_value_t(const Collection &storage, size_t position, uint32_t msb_mask = 0xff) {
            uint16_t msb = storage[position++] & msb_mask;
            uint16_t lsb = storage[position];

            static const uint32_t msb_shift = 8;
            value = (msb << msb_shift) + lsb;
        }

        uint16_t value;
    };
}
