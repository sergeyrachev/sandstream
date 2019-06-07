#pragma once

#include "callback_ts_packet.h"
#include "section_header.h"

#include <vector>

namespace challenge {

    class parser_psi_t : public callback_ts_packet {
    public:
        void put(ts_packet_t packet) override;

    protected:
        struct parse_result_t{
            bool is_success;
            size_t consumed_bytes;
            section_header_t header;
        };

    protected:
        virtual void on_payload(const uint8_t* data, size_t available) = 0;

    protected:
        static size_t skip_padding_bytes(const uint8_t* data, size_t available);
        static parse_result_t try_parse_header(const uint8_t* data, size_t available);

    private:
        std::vector<uint8_t> section_data;
    };
}
