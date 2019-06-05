#pragma once

#include "mock.h"
#include "callback_es.h"

namespace mock{
    class callback_es_t: public challenge::callback_es_t{
    public:
        callback_es_t();
        ~callback_es_t() final;
        MOCK_METHOD2(on_data, void(const uint8_t* data, size_t size));
    };
}
