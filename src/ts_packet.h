#pragma once

#include "buffer.h"

#include <cstddef>
#include <cstdint>

namespace challenge {
    class ts_packet_t{
    public:
        static const size_t ts_packet_size = 188;
        static const uint8_t ts_sync_byte = 0x47;

    public:
        ts_packet_t(const uint8_t* data, size_t available_bytes);
        ts_packet_t(uint16_t pid, bool payload_unit_start_indicator, const_buffer_t payload);
        ts_packet_t();

        uint16_t pid{};
        bool payload_unit_start_indicator{};
        const_buffer_t payload{};
    };
}
