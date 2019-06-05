#include "parser_pmt.h"

#include "masked_two_bytes_value.h"

#include "parser_pmt_details.h"

challenge::parser_pmt_t::parser_pmt_t(challenge::callback_pmt_t &callback) : callback(callback) {

}

size_t challenge::parser_pmt_t::parse_payload(const size_t payload_size, const challenge::storage_t &storage, const size_t position) {
    return details::parse_pmt_payload(payload_size, storage, position, tracks);
}

void challenge::parser_pmt_t::update() {
    callback.on_pmt(tracks);
    tracks.clear();
}
