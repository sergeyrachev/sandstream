#pragma once

#include "buffer.h"

namespace challenge{
    class source_data_t{
    public:
        virtual ~source_data_t() = default;
        virtual mutable_buffer_t request() = 0;
        virtual void confirm(mutable_buffer_t buffer) = 0;
    };
}
