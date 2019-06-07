#pragma once

#include "buffer.h"

#include <cstddef>
#include <cstdint>

namespace challenge {
    class callback_es_t{
    public:
        virtual ~callback_es_t() = default;
        virtual void on_data(challenge::const_buffer_t) = 0;
    };
}
