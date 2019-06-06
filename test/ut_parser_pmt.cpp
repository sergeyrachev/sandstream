#include "gtest/gtest.h"

#include "parser_pmt.h"
#include "parser_pmt_details.h"

#include "mock_callback_pmt.h"

#include <vector>

namespace uncovered{
    class parser_pmt_t : public challenge::parser_pmt_t {
    public:
        using challenge::parser_pmt_t::parser_pmt_t;
        using challenge::parser_pmt_t::update;
        using challenge::parser_pmt_t::parse_payload;

        using challenge::parser_pmt_t::tracks;
    };
}
using uncovered::parser_pmt_t;
using mock::callback_pmt_t;
using challenge::pmt_t;
using challenge::storage_t;

TEST(parser_pmt, update){
    mock::callback_pmt_t callback;
    ON_CALL(callback, on_pmt(_)).WillByDefault(Return());

    pmt_t meaninfull_pmt{{0, 1}, {2, 3}};
    pmt_t empty_pmt{};
    EXPECT_CALL(callback, on_pmt(meaninfull_pmt)).WillOnce(Return());
    EXPECT_CALL(callback, on_pmt(empty_pmt)).WillOnce(Return());

    parser_pmt_t parser(callback);
    parser.tracks = meaninfull_pmt;
    parser.update();
    parser.update();
}

TEST(parser_pmt, details_parse_payload){
    using details::parse_pmt_payload;

    static const size_t payload_size = 24;
    static const size_t position = 2;

    const std::vector<uint8_t> payload{0x00, 0x00,
                                       0xe0, 0x21, 0xf0, 0x00,
                                       0x0f, 0xe0, 0x22, 0xf0, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0x1b, 0xe0, 0x21, 0xf0, 0x00, 0xab, 0xab, 0xab, 0xab};
    challenge::pmt_t pmts;
    auto result = parse_pmt_payload(payload_size, payload, position, pmts);

    ASSERT_EQ(payload_size + position, result);

    static const uint16_t expected_pid_first = 0x22;
    static const uint16_t expected_stream_type_first = 0x0f;
    static const uint16_t expected_pid_second = 0x21;
    static const uint16_t expected_stream_type_second = 0x1b;
    pmt_t expected_pat{{ expected_pid_first, expected_stream_type_first }, { expected_pid_second, expected_stream_type_second }};
    ASSERT_EQ(expected_pat, pmts);
}

TEST(parser_pmt, parse_payload_single) {
    using details::parse_pmt_payload;

    static const size_t payload_size = 19;
    static const size_t position = 2;

    const std::vector<uint8_t> payload{0x00, 0x00,
                                       0xe0, 0x21, 0xf0, 0x00,
                                       0x0f, 0xe0, 0x22, 0xf0, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0xab, 0xab, 0xab, 0xab};

    static const uint16_t expected_pid = 0x22;
    static const uint16_t expected_stream_type = 0x0f;
    pmt_t expected_pat{{expected_pid, expected_stream_type}};

    storage_t storage;
    storage.push_back({nullptr, payload.data(), payload.size()});

    mock::callback_pmt_t callback;
    parser_pmt_t parser(callback);
    auto result = parser.parse_payload(payload_size, storage, position);
    ASSERT_EQ(payload_size + position, result);
    ASSERT_EQ(expected_pat, parser.tracks);
}

TEST(parser_pmt, parse_payload_accumulation){
    using details::parse_pmt_payload;

    static const size_t payload_size = 19;

    static const size_t position_first = 2;
    const std::vector<uint8_t> payload_first{0x00, 0x00,
                                       0xe0, 0x21, 0xf0, 0x00,
                                       0x0f, 0xe0, 0x22, 0xf0, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0xab, 0xab, 0xab, 0xab};
    static const uint16_t expected_pid_first = 0x22;
    static const uint16_t expected_stream_type_first = 0x0f;

    static const size_t position_second = 2;
    const std::vector<uint8_t> payload_second{0x00, 0x00,
                                             0xe0, 0x21, 0xf0, 0x00,
                                             0x1b, 0xe0, 0x45, 0xf0, 0x06, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                             0xab, 0xab, 0xab, 0xab};
    static const uint16_t expected_pid_second = 0x45;
    static const uint16_t expected_stream_type_second = 0x1b;
    pmt_t expected_pat{{ expected_pid_first, expected_stream_type_first },{ expected_pid_second, expected_stream_type_second }};

    mock::callback_pmt_t callback;
    parser_pmt_t parser(callback);

    storage_t storage;
    storage.push_back({nullptr, payload_first.data(), payload_first.size()});

    auto result = parser.parse_payload( payload_size, storage, position_first);
    storage.pop_front(result);

    storage.push_back({nullptr, payload_second.data(), payload_second.size()});

    result = parser.parse_payload( payload_size, storage, position_second);

    ASSERT_EQ(expected_pat, parser.tracks);
    ASSERT_EQ(payload_size + position_second, result);
}
