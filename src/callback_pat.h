#pragma once

#include "pat.h"

namespace challenge {

    ///
    /// @brief Internal interface to handle PAT sections; implemented by @see demuxer_t
    ///
    class callback_pat_t {
    public:
        virtual ~callback_pat_t() = default;
        virtual void on_pat(const pat_t &update) = 0;
    };
}
