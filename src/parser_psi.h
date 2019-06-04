#pragma once

#include "callback_ts_packet.h"
#include "section_header.h"
#include "storage.h"

namespace challenge {

    class parser_psi_t : public callback_ts_packet {
    public:
        void put(std::unique_ptr<ts_packet_t> packet) override;

    protected:
        virtual size_t parse_payload(const section_header_t &header, size_t payload_size, const storage_t &storage, size_t position);
        virtual void update();
        static size_t skip_padding_bytes(const storage_t &storage, size_t position);
        static std::tuple<bool, section_header_t> try_parse(const storage_t &storage, size_t &consumed_bytes);

    private:
        storage_t storage;
    };

}
