#include "gtest/gtest.h"

#include "parser_pat.h"
#include "details_parser_pat.h"

#include "mock_callback_pat.h"

#include <vector>

namespace uncovered{
    class parser_pat_t : public challenge::parser_pat_t {
    public:
        using challenge::parser_pat_t::parser_pat_t;
        using challenge::parser_pat_t::update;
        using challenge::parser_pat_t::parse_payload;

        using challenge::parser_pat_t::services;
    };
}
using uncovered::parser_pat_t;
using mock::callback_pat;
using challenge::pat_t;
using challenge::storage_t;

TEST(parser_pat, update){
    mock::callback_pat callback;
    ON_CALL(callback, on_pat(_)).WillByDefault(Return());

    pat_t meaninfull_pat{{0, 1}, {2, 3}};
    pat_t empty_pat{};
    EXPECT_CALL(callback, on_pat(meaninfull_pat)).WillOnce(Return());
    EXPECT_CALL(callback, on_pat(empty_pat)).WillOnce(Return());

    parser_pat_t parser(callback);
    parser.services = meaninfull_pat;
    parser.update();
    parser.update();
}

TEST(parser_pat, details_parse_payload){
    using details::parse_pat_payload;

    static const size_t payload_size = 8;
    static const size_t position = 4;

    const std::vector<uint8_t> payload{0x08, 0x09, 0x0a, 0x0b, 0x01, 0x02, 0x03, 0x04, 0xad, 0xad, 0xad, 0xad};
    auto result = parse_pat_payload(payload_size, payload, position);

    ASSERT_EQ(payload_size + position, std::get<0>(result));

    static const uint16_t expected_pid = 0x0304;
    static const uint16_t expected_program_number = 0x0102;
    pat_t expected_pat{{ expected_pid, expected_program_number }};
    ASSERT_EQ(expected_pat, std::get<1>(result));
}

TEST(parser_pat, parse_payload_single) {
    using details::parse_pat_payload;

    static const size_t payload_size = 8;
    static const size_t position = 4;

    const std::vector<uint8_t> payload{0x08, 0x09, 0x0a, 0x0b, 0x01, 0x02, 0x03, 0x04, 0xad, 0xad, 0xad, 0xad};
    static const uint16_t expected_pid = 0x0304;
    static const uint16_t expected_program_number = 0x0102;
    pat_t expected_pat{{expected_pid, expected_program_number}};

    storage_t storage;
    storage.push_back({nullptr, payload.data(), payload.size()});

    mock::callback_pat callback;
    parser_pat_t parser(callback);
    auto result = parser.parse_payload({}, payload_size, storage, position);
    ASSERT_EQ(payload_size + position, result);
    ASSERT_EQ(expected_pat, parser.services);
}

TEST(parser_pat, parse_payload_accumulation){
    using details::parse_pat_payload;

    static const size_t payload_size = 8;

    static const size_t position_first = 4;
    const std::vector<uint8_t> payload_first{0x08, 0x09, 0x0a, 0x0b, 0x01, 0x02, 0xff, 0x04, 0xad, 0xad, 0xad, 0xad};
    static const uint16_t expected_pid_first = 0x1f04;
    static const uint16_t expected_program_number_first = 0x0102;

    static const size_t position_second = 0;
    const std::vector<uint8_t> payload_second{0x08, 0x09, 0x0a, 0x0b, 0xad, 0xad, 0xad, 0xad};
    static const uint16_t expected_pid_second = 0x0a0b;
    static const uint16_t expected_program_number_second = 0x0809;
    pat_t expected_pat{{ expected_pid_first, expected_program_number_first },{ expected_pid_second, expected_program_number_second }};

    mock::callback_pat callback;
    parser_pat_t parser(callback);

    storage_t storage;
    storage.push_back({nullptr, payload_first.data(), payload_first.size()});

    auto result = parser.parse_payload({}, payload_size, storage, position_first);
    storage.pop_front(result);

    storage.push_back({nullptr, payload_second.data(), payload_second.size()});

    result = parser.parse_payload({}, payload_size, storage, position_second);
    ASSERT_EQ(expected_pat, parser.services);
}
