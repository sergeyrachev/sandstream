#include "gtest/gtest.h"

#include "parser_pmt.h"

#include "mock_callback_pmt.h"
#include "matcher.h"

#include <vector>

namespace uncovered{
    class parser_pmt_t : public challenge::parser_pmt_t {
    public:
        using challenge::parser_pmt_t::parser_pmt_t;
        using challenge::parser_pmt_t::on_payload;
    };
}
using uncovered::parser_pmt_t;
using mock::callback_pmt_t;
using challenge::pmt_t;

TEST(parser_pmt, parse_payload){

    static const uint16_t expected_pid_first = 0x22;
    static const uint16_t expected_stream_type_first = 0x0f;
    static const uint16_t expected_pid_second = 0x21;
    static const uint16_t expected_stream_type_second = 0x1b;
    pmt_t expected_pat{{ expected_pid_first, expected_stream_type_first }, { expected_pid_second, expected_stream_type_second }};

    const std::vector<uint8_t> payload{0xe0, 0x21, 0xf0, 0x00,
                                       0x0f, 0xe0, 0x22, 0xf0, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0x1b, 0xe0, 0x21, 0xf0, 0x00, 0xab, 0xab, 0xab, 0xab};

    mock::callback_pmt_t callback;
    EXPECT_CALL(callback, on_pmt(expected_pat)).Times(1);

    parser_pmt_t parser(callback);
    parser.on_payload(payload.data(), payload.size());
}
