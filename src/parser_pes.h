#pragma once

#include "callback_ts_packet.h"
#include "callback_es.h"

#include <memory>

namespace challenge {

    ///
    /// @brief Implements PES packet header parsing and elementary stream payload extraction. Doesn't support split PES header!
    ///
    class parser_pes_t : public callback_ts_packet_t {
    public:
        explicit parser_pes_t(std::unique_ptr<callback_es_t> consumer);

        void put(ts_packet_t packet) final;

    protected:
        struct parse_result_t {
            bool is_success;
            size_t consumed_bytes;
        };
        static parse_result_t try_parse_header(const uint8_t* data, size_t available);

    private:
        std::unique_ptr<callback_es_t> consumer;
    };
}
