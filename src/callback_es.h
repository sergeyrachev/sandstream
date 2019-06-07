#pragma once

#include "buffer.h"

#include <cstddef>
#include <cstdint>

namespace challenge {

    ///
    /// @brief user-side interface to consume demultiplexed data;
    /// user should implement this interface and provide @see callback_es_factory_t
    /// instance to demuxer_t
    ///
    class callback_es_t{
    public:
        virtual ~callback_es_t() = default;
        virtual void on_data(challenge::const_buffer_t) = 0;
    };
}
