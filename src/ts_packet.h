#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace challenge {
    class ts_packet_t{
    public:
        static const uint8_t ts_sync_byte = 0x47;
        static const size_t ts_packet_size = 188;

    public:
        ts_packet_t(const uint8_t* data, size_t available_bytes);

        uint16_t pid;
        bool payload_unit_start_indicator;
        std::vector<uint8_t> payload;
    };
}
