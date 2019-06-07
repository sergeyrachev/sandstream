#pragma once

#include "matcher.h"
#include "callback_es.h"

namespace mock{
    class callback_es_t: public challenge::callback_es_t{
    public:
        callback_es_t();
        ~callback_es_t() final;
        MOCK_METHOD1(on_data, void(challenge::const_buffer_t));
    };
}
