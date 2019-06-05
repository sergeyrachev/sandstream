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
        struct parse_result_t {
            bool is_success;
            size_t consumed_bytes;
        };
        static parse_result_t try_parse(const storage_t &storage, size_t initial_position);

    private:
        std::unique_ptr<callback_es_t> consumer;
        storage_t storage;
    };
}
