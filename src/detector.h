#pragma once

#include <cstdint>
#include <vector>

namespace challenge {
    class detector_t{
    public:
        detector_t() = default;

        size_t detect(const uint8_t* data, size_t available_bytes);

        std::vector<uint32_t> elementary_stream_pids{};

    private:
        void parse_pat(const uint8_t* data, size_t available_bytes);
        void parse_pmt(const uint8_t* data, size_t available_bytes);
        uint32_t pmt_pid{0};
    };
}
