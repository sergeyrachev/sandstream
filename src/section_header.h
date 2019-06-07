#pragma once

#include <cstddef>
#include <cstdint>

namespace challenge {

    ///
    /// @brief Naive section header model; is used by @see parser_psi_t
    ///
    struct section_header_t {
        uint16_t section_length;
    };
}
