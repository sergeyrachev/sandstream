#pragma once

#include <cstddef>
#include <cstdint>


namespace challenge{
    class ts_packet_t{
        static const uint8_t ts_sync_byte = 0x47;
    public:
        static const size_t ts_packet_size = 188;
    public:
        ts_packet_t(const uint8_t* data, size_t available_bytes);

        static size_t sync(const uint8_t* data, size_t available_bytes);

        bool payload_unit_start_indicator;
        uint32_t pid;

        const uint8_t* adaptation_field;
        uint32_t adaptation_field_size;

        const uint8_t* payload;
        uint32_t payload_size;
    };
}
