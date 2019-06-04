#pragma once

#include "mock.h"
#include "callback_pmt.h"

namespace mock{
    class callback_pmt : public challenge::callback_pmt_t{
    public:
        callback_pmt();
        ~callback_pmt() final;
        MOCK_METHOD1(on_pmt, void(const challenge::pmt_t &update));
    };
}
