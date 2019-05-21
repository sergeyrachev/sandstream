#pragma once

#include <cstdint>
#include <vector>

namespace challenge{
    class pes_packet_t{
        static const uint8_t packet_start_code_prefix_length = 3;
        static const uint8_t stream_id_length = 1;
        static const uint8_t PES_packet_length_length = 2;
    public:
        pes_packet_t(const uint8_t* data, size_t available_bytes);

        void assign(const uint8_t* data, size_t available_bytes);

        const uint8_t* payload;
        uint32_t payload_size;

    private:
        bool parse_header(const uint8_t* data, size_t available_bytes);

    private:
        std::vector<uint8_t> storage{};

        uint8_t stream_id{};
        size_t pes_packet_length{};
    };

}
