#pragma once

#include "matcher.h"
#include "callback_pat.h"

namespace mock{
    class callback_pat_t : public challenge::callback_pat_t{
    public:
        callback_pat_t();
        ~callback_pat_t() final;
        MOCK_METHOD1(on_pat, void(const challenge::pat_t &update));
    };
}
