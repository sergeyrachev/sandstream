#pragma once

#include "callback_es.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace challenge {

    ///
    /// @brief User-defined factory; demuxer_t instance uses it to obtain elementary stream data consumer when new elementary stream detected.
    /// @see callback_es_t
    ///
    class callback_es_factory_t{
    public:
        virtual ~callback_es_factory_t() = default;
        virtual std::unique_ptr<callback_es_t> create(uint8_t stream_type, uint16_t pid ) = 0;
    };
}
