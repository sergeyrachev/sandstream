#pragma once

#include "parser_psi.h"
#include "callback_pmt.h"
#include "section_header.h"

namespace challenge {

    ///
    /// @brief  Implements PMT section payload parsing; doesn't verify section version or table completeness by section number values!
    ///
    class parser_pmt_t : public parser_psi_t {
    public:
        explicit parser_pmt_t(callback_pmt_t &callback);

    protected:
        void on_payload(const uint8_t* data, size_t available) final;

        static pmt_t parse_pmt_payload( const uint8_t* data, size_t payload_size );
    protected:
        callback_pmt_t &callback;
    };
}
