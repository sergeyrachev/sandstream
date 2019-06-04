#pragma once

#include "pat.h"

namespace challenge {
    class callback_pat_t {
    public:
        virtual ~callback_pat_t() = default;
        virtual void on_pat(const pat_t &update) = 0;
    };
}
