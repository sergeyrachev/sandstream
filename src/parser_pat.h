#pragma once

#include "parser_psi.h"
#include "callback_pat.h"

namespace challenge {

    class parser_pat_t : public parser_psi_t {
    public:
        explicit parser_pat_t(callback_pat_t &handler);

    protected:
        size_t parse_payload(size_t payload_size, const storage_t &storage, size_t position) final;

        void update() override {
            handler.on_pat(services);
            services.clear();
        }

    protected:
        callback_pat_t &handler;
        pat_t services{};
    };
}
