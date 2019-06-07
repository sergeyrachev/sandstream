#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge{
    struct mutable_buffer_t{
        uint8_t* data;
        size_t size;
    };

    struct const_buffer_t{
        const uint8_t* data;
        size_t size;
    };
}
