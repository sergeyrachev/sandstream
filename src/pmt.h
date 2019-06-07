#pragma once

#include <cstddef>
#include <cstdint>
#include <map>

namespace challenge{

    ///
    /// @brief Naive PMT model; contains PID and stream type from PMT;
    ///
    using pid_t = uint16_t;
    using stream_type_t = uint8_t;
    using pmt_t = std::map<pid_t, stream_type_t>;
}
