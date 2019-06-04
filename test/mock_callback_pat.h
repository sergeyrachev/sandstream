#pragma once

#include "mock.h"
#include "callback_pat.h"

namespace mock{
    class callback_pat : public challenge::callback_pat_t{
    public:
        callback_pat();
        ~callback_pat() final;
        MOCK_METHOD1(on_pat, void(const challenge::pat_t &update));
    };
}
