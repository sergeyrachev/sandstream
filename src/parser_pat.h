#pragma once

#include "parser_psi.h"
#include "callback_pat.h"

namespace challenge {

    class parser_pat_t : public parser_psi_t {
    public:
        explicit parser_pat_t(callback_pat_t &handler);

    protected:
        void on_payload(const uint8_t* data, size_t available) final;

        static pat_t parse_pat_payload( const uint8_t* data, size_t available );

    protected:
        callback_pat_t &callback;
    };
}
