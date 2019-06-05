#pragma once

#include "callback_ts_packet.h"
#include "section_header.h"
#include "storage.h"

namespace challenge {

    class parser_psi_t : public callback_ts_packet {
    public:
        void put(std::unique_ptr<ts_packet_t> packet) override;

    protected:
        struct parse_result_t{
            bool is_success;
            size_t consumed_bytes;
            section_header_t header;
        };

    protected:
        virtual size_t parse_payload( size_t payload_size, const storage_t &storage, size_t position) = 0;
        virtual void update() = 0;

    protected:
        static size_t skip_padding_bytes(const storage_t &storage, size_t position);
        static parse_result_t try_parse(const storage_t &storage, size_t initial_position);

    private:
        storage_t storage;
    };
}
