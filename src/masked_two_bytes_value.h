#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge {
    struct masked_two_bytes_value_t {
        static const size_t two_byte_value_length = 2;

        masked_two_bytes_value_t(const uint8_t* data, uint32_t msb_mask = 0xff) {
            uint16_t msb = *data++ & msb_mask;
            uint16_t lsb = *data;

            static const uint32_t msb_shift = 8;
            value = (msb << msb_shift) + lsb;
        }

        uint16_t value;
    };
}
