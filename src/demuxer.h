#pragma once

#include "pes_packet.h"

#include <cstdint>
#include <memory>
#include <ostream>

namespace challenge{
    class demuxer_t{
    public:
        demuxer_t() = default;

        size_t consume(const uint8_t* data, size_t available_bytes, uint32_t pid, std::ostream& sink);

    private:
        std::unique_ptr<pes_packet_t> stream;
    };
}
