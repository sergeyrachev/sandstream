#pragma once

#include "ts_packet.h"

#include <memory>

namespace challenge {
    class callback_ts_packet {
    public:
        virtual ~callback_ts_packet() = default;
        virtual void put(std::unique_ptr<ts_packet_t>) = 0;
    };
}
