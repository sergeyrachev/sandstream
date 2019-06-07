#include "gtest/gtest.h"

#include "parser_pat.h"

#include "mock_callback_pat.h"
#include "matcher.h"

#include <vector>

namespace uncovered{
    class parser_pat_t : public challenge::parser_pat_t {
    public:
        using challenge::parser_pat_t::parser_pat_t;
        using challenge::parser_pat_t::on_payload;
    };
}
using uncovered::parser_pat_t;
using mock::callback_pat_t;
using challenge::pat_t;

TEST(parser_pat, parse_payload){

    const std::vector<uint8_t> payload{0x01, 0x02, 0x03, 0x04, 0x0a, 0x0b, 0x0c, 0x0d, 0xad, 0xad, 0xad, 0xad};

    static const uint16_t expected_pid_first = 0x0304;
    static const uint16_t expected_program_number_first = 0x0102;
    static const uint16_t expected_pid_second = 0x0c0d;
    static const uint16_t expected_program_number_second = 0x0a0b;
    challenge::pat_t expected_pat{{ expected_pid_first, expected_program_number_first }, { expected_pid_second, expected_program_number_second }};

    mock::callback_pat_t callback;
    EXPECT_CALL(callback, on_pat(expected_pat)).Times(1);

    parser_pat_t parser(callback);
    parser.on_payload(payload.data(), payload.size());
}
