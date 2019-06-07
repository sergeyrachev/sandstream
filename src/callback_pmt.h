#pragma once

#include "pmt.h"

namespace challenge {

    ///
    /// @brief Internal interface to handle PMT sections; implemented by @see demuxer_t
    ///
    class callback_pmt_t {
    public:
        virtual ~callback_pmt_t() = default;
        virtual void on_pmt(const pmt_t& update) = 0;
    };
}
