#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge {
    struct section_header_t {
        static const size_t section_header_length = 3;
        static const size_t section_ext_header_length = 5;
        uint16_t section_length;
        uint8_t section_number;
        uint8_t last_section_number;
    };
}
