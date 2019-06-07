#include "gtest/gtest.h"

#include "parser_psi.h"
#include "mock_parser_psi.h"

#include <vector>
#include <functional>

namespace uncovered{
    class parser_psi_t : public challenge::parser_psi_t {
    public:
        using challenge::parser_psi_t::skip_padding_bytes;
        using challenge::parser_psi_t::try_parse_header;
        using challenge::parser_psi_t::parse_result_t;
    };
}
using uncovered::parser_psi_t;

TEST(parser_psi, skip_padding){

    std::vector<uint8_t> padded_payload{0xff, 0xff, 0xff, 0x04, 0x05};

    auto result = parser_psi_t::skip_padding_bytes(padded_payload.data(), padded_payload.size());
    ASSERT_EQ(3, result);
}

TEST(parser_psi, try_parse_success){
    const std::vector<uint8_t> payload{0x00, 0xb0, 0x0d, 0x41, 0x57, 0xc1, 0x01, 0x02, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91 };

    size_t initial_position = 0;
    auto result = parser_psi_t::try_parse_header(payload.data(), payload.size());

    ASSERT_TRUE(result.is_success);
    ASSERT_EQ( 8, result.consumed_bytes);

    ASSERT_EQ(0x0d, result.header.section_length);
}

TEST(parser_psi, try_parse_partial){
    const std::vector<uint8_t> payload_1{ 0x00, 0xb0, };
    const std::vector<uint8_t> payload_2{ 0x00, 0xb0, 0x0d, 0x41, };

    size_t initial_position = 0;
    auto result = parser_psi_t::try_parse_header( payload_1.data(), payload_1.size());

    ASSERT_FALSE(result.is_success);
    ASSERT_EQ( 0, result.consumed_bytes);

    result = parser_psi_t::try_parse_header(payload_2.data(), payload_2.size());

    ASSERT_FALSE(result.is_success);
    ASSERT_EQ( 0,result.consumed_bytes);
}

TEST(parser_psi, short_section_parsing){
    mock::parser_psi_t parser;

    const size_t payload_size = 8;
    const size_t payload_position = 8;
    const std::vector<uint8_t> payload{ 0x00, 0x00, 0xb0, 0x0d, 0x41, 0x57, 0xc1, 0x01, 0x01, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    static const bool pusi_flag = true;
    challenge::ts_packet_t pusi_packet(0, pusi_flag, {payload.data(), payload.size()});

    EXPECT_CALL(parser, on_payload(_, payload_position)).Times(1);

    parser.put(pusi_packet);
}

TEST(parser_psi, long_section_parsing){
    mock::parser_psi_t parser;

    const size_t payload_size = 16;
    const size_t payload_position = 8;
    const std::vector<uint8_t> payload_part_1{ 0x00, 0x00, 0xb0, 0x0d + 8, 0x41, 0x57, 0xc1, 0x01, 0x01, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    const std::vector<uint8_t> payload_part_2{                                                           0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};

    static const bool pusi_flag = true;
    challenge::ts_packet_t pusi_packet(0, pusi_flag, {payload_part_1.data(), payload_part_1.size()});
    challenge::ts_packet_t regular_packet(0, !pusi_flag, {payload_part_2.data(), payload_part_2.size()});

    EXPECT_CALL(parser, on_payload(_, payload_position)).Times(1);

    parser.put(pusi_packet);
    parser.put(regular_packet);
}

TEST(parser_psi, two_section_parsing){
    mock::parser_psi_t parser;

    const size_t payload_size_section_1 = 10;
    const size_t payload_position_section_1 = 8;
    const std::vector<uint8_t> payload_part_1{ 0x00, 0x00, 0xb0, 0x0d + 2, 0x41, 0x57, 0xc1, 0x01, 0x01, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};

    const size_t payload_size_section_2 = 8;
    const size_t payload_position_section_2 = 8;
    const std::vector<uint8_t> payload_part_2{ 0x02, 0xbb, 0x0cc, 0x00, 0xb0, 0x0d, 0x41, 0x57, 0xc1, 0x01, 0x01, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    static const bool pusi_flag = true;
    challenge::ts_packet_t first_packet(0, pusi_flag, {payload_part_1.data(), payload_part_1.size()});
    challenge::ts_packet_t second_packet(0, pusi_flag, {payload_part_2.data(), payload_part_2.size()});

    EXPECT_CALL(parser, on_payload(_, payload_size_section_1)).Times(1);
    EXPECT_CALL(parser, on_payload(_, payload_size_section_2)).Times(1);

    parser.put(first_packet);
    parser.put(second_packet);
}
