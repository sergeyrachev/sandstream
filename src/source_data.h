#pragma once

#include "buffer.h"

namespace challenge{

    ///
    /// @brief Input layer abstraction; user requests mutable buffer,
    /// fill data block and update size field with actual amount of data;
    /// then call confirm to initiate to pass data back
    ///
    class source_data_t{
    public:
        virtual ~source_data_t() = default;
        virtual mutable_buffer_t request() = 0;
        virtual void confirm(mutable_buffer_t buffer) = 0;
    };
}
