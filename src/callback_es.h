#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge {
    class callback_es_t{
    public:
        virtual ~callback_es_t() = default;
        virtual void on_data(const uint8_t* data, size_t size) = 0;
    };
}
