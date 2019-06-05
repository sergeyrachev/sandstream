#include "parser_pat.h"

#include "parser_pat_details.h"

challenge::parser_pat_t::parser_pat_t(challenge::callback_pat_t &handler) : handler(handler) {}

size_t challenge::parser_pat_t::parse_payload(const size_t payload_size, const storage_t &storage, const size_t position) {
    return details::parse_pat_payload(payload_size, storage, position, services);
}
