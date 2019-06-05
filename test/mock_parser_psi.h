#pragma once

#include "mock.h"

#include "parser_psi.h"

namespace mock{
    class parser_psi_t : public challenge::parser_psi_t{
    public:
        parser_psi_t();
        ~parser_psi_t();
        MOCK_METHOD3( parse_payload, size_t( size_t payload_size, const challenge::storage_t &storage, size_t position));
        MOCK_METHOD0(update, void());
    };
}
