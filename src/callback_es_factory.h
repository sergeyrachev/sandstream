#pragma once

#include "callback_es.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace challenge {
    class callback_es_factory_t{
    public:
        virtual ~callback_es_factory_t() = default;
        virtual std::unique_ptr<callback_es_t> create(uint8_t stream_type, uint16_t pid ) = 0;
    };
}
