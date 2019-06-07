#pragma once

#include "ts_packet.h"

namespace challenge{

    ///
    /// @brief Internal interface to obtain TS packets one-by-one
    ///
    class source_packet_t{
    public:
        virtual ~source_packet_t() = default;
        virtual bool is_depleted() const = 0;
        virtual challenge::ts_packet_t get() = 0;
    };
}
