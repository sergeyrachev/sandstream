#pragma once

#include "callback_ts_packet.h"
#include "callback_es.h"
#include "storage.h"

#include <memory>

namespace challenge {

    class parser_pes_t : public callback_ts_packet {
    public:
        explicit parser_pes_t(std::unique_ptr<callback_es_t> consumer);

        void put(std::unique_ptr<ts_packet_t> packet) final;

    protected:
        static bool try_parse(const storage_t &storage, size_t &position);

    private:
        std::unique_ptr<callback_es_t> consumer;
        storage_t storage;
    };
}
