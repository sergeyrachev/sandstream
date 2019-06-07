#pragma once

#include "matcher.h"

#include "parser_psi.h"

namespace mock{
    class parser_psi_t : public challenge::parser_psi_t{
    public:
        parser_psi_t();
        ~parser_psi_t();
        MOCK_METHOD2( on_payload, void( const uint8_t* data, size_t available));
    };
}
