#pragma once

#include <cstddef>
#include <cstdint>
#include <map>

namespace challenge{

    ///
    /// @brief Naive PAT model; contains PID and Program Number;
    ///
    using pid_t = uint16_t;
    using program_number_t = uint16_t;
    using pat_t = std::map<pid_t, program_number_t>;
}
