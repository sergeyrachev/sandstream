#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge{
    ///
    /// @brief Provides writable memory block access; doesn't own the allocated memory, only refer to it
    ///
    struct mutable_buffer_t{
        uint8_t* data;
        size_t size;
    };

    ///
    /// @brief Provides non-writable memory block access; doesn't own the allocated memory, only refer to it
    ///
    struct const_buffer_t{
        const uint8_t* data;
        size_t size;
    };
}
