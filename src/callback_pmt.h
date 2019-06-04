#pragma once

#include "pmt.h"

namespace challenge {
    class callback_pmt_t {
    public:
        virtual ~callback_pmt_t() = default;
        virtual void on_pmt(const pmt_t& update) = 0;
    };
}
