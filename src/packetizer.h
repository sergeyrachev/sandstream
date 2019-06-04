#pragma once

#include "ts_packet.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <vector>
#include <list>

namespace challenge{
    class packetizer_t{
    public:
        std::unique_ptr<ts_packet_t> get();

        void put(const uint8_t* data, size_t size);

    protected:
        void store_partial_packet(const uint8_t* data, size_t size);
        size_t recover_partial_packet(const uint8_t* data, size_t size);

        static std::unique_ptr<ts_packet_t> parse_single_packet(const uint8_t* data, size_t available_bytes);

    private:
        std::list<std::unique_ptr<ts_packet_t>> sequence;
        std::vector<uint8_t> partial_packet;
    };
}
