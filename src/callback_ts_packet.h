#pragma once

#include "ts_packet.h"

#include <memory>

namespace challenge {

    ///
    /// @brief Internal interface to handle TS packets; implemented by PSI parsers and PES parser
    /// @see parser_pes_t, @see parser_psi_t
    ///
    class callback_ts_packet_t {
    public:
        virtual ~callback_ts_packet_t() = default;
        virtual void put(ts_packet_t) = 0;
    };
}
