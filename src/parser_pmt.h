#pragma once

#include "parser_psi.h"
#include "callback_pmt.h"
#include "section_header.h"
#include "storage.h"

namespace challenge {
    class parser_pmt_t : public parser_psi_t {
    public:
        explicit parser_pmt_t(callback_pmt_t &callback);

    protected:
        size_t parse_payload(const section_header_t &header, size_t payload_size, const storage_t &storage, size_t position) final;
        void update() override;

    private:
        callback_pmt_t &callback;
        pmt_t tracks;
    };
}
