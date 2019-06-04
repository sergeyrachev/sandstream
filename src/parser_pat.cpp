#include "parser_pat.h"

#include "details_parser_pat.h"

challenge::parser_pat_t::parser_pat_t(challenge::callback_pat_t &handler) : handler(handler) {}

size_t challenge::parser_pat_t::parse_payload(const challenge::section_header_t &header, const size_t payload_size, const storage_t &storage, const size_t position) {
    static const uint8_t size_t_index = 0;
    static const uint8_t pat_t_index = 1;

    auto result = details::parse_pat_payload(payload_size, storage, position);
    const auto& update = std::get<pat_t_index>(result);
    services.insert(update.begin(), update.end());
    return std::get<size_t_index>(result);
}
