#include "gtest/gtest.h"

#include "parser_psi.h"
#include "mock_parser_psi.h"

#include <vector>
#include <functional>

namespace uncovered{
    class parser_psi_t : public challenge::parser_psi_t {
    public:
        using challenge::parser_psi_t::skip_padding_bytes;
        using challenge::parser_psi_t::try_parse;
        using challenge::parser_psi_t::parse_result_t;
    };
}
using uncovered::parser_psi_t;
using challenge::storage_t;

TEST(parser_psi, skip_padding){

    std::vector<uint8_t> padded_payload{0xff, 0xff, 0xff, 0x04, 0x05};
    storage_t storage;
    storage.push_back({nullptr, padded_payload.data(), padded_payload.size()});

    auto result = parser_psi_t::skip_padding_bytes(storage, 0);
    ASSERT_EQ(3, result);

    result = parser_psi_t::skip_padding_bytes(storage, 1);
    ASSERT_EQ(3, result);

    result = parser_psi_t::skip_padding_bytes(storage, 3);
    ASSERT_EQ(3, result);

    result = parser_psi_t::skip_padding_bytes(storage, 4);
    ASSERT_EQ(4, result);
}

TEST(parser_psi, try_parse_success){
    const std::vector<uint8_t> payload{0x00, 0xb0, 0x0d, 0x41, 0x57, 0xc1, 0x01, 0x02, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91 };
    storage_t storage;
    storage.push_back({nullptr, payload.data(), payload.size()});

    size_t initial_position = 0;
    auto result = parser_psi_t::try_parse(storage, initial_position);

    ASSERT_TRUE(result.is_success);
    ASSERT_EQ( 8, result.consumed_bytes);

    ASSERT_EQ(0x0d, result.header.section_length);
    ASSERT_EQ(0x01, result.header.section_number);
    ASSERT_EQ(0x02, result.header.last_section_number);
}

TEST(parser_psi, try_parse_partial){
    const std::vector<uint8_t> payload_part_1{ 0x00, 0xb0, };
    const std::vector<uint8_t> payload_part_2{ 0x0d, 0x41, };
    const std::vector<uint8_t> payload_part_3{ 0x57, 0xc1, 0x01, 0x02, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91 };

    storage_t storage;
    storage.push_back({nullptr, payload_part_1.data(), payload_part_1.size()});

    size_t initial_position = 0;
    auto result = parser_psi_t::try_parse(storage, initial_position);

    ASSERT_FALSE(result.is_success);
    ASSERT_EQ( 0, result.consumed_bytes);

    storage.push_back({nullptr, payload_part_2.data(), payload_part_2.size()});
    result = parser_psi_t::try_parse(storage, initial_position);

    ASSERT_FALSE(result.is_success);
    ASSERT_EQ( 0,result.consumed_bytes);

    storage.push_back({nullptr, payload_part_3.data(), payload_part_3.size()});
    result = parser_psi_t::try_parse(storage, initial_position);

    ASSERT_TRUE(result.is_success);
    ASSERT_EQ( 8, result.consumed_bytes);
    ASSERT_EQ(0x0d, result.header.section_length);
    ASSERT_EQ(0x01, result.header.section_number);
    ASSERT_EQ(0x02, result.header.last_section_number);
}

TEST(parser_psi, short_last_section_parsing){
    mock::parser_psi_t parser;

    const size_t payload_size = 8;
    const size_t payload_position = 8;
    const std::vector<uint8_t> payload{ 0x00, 0x00, 0xb0, 0x0d, 0x41, 0x57, 0xc1, 0x01, 0x01, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    static const bool pusi_flag = true;
    std::unique_ptr<challenge::ts_packet_t> pusi_packet(new challenge::ts_packet_t(0, pusi_flag, payload));

    ON_CALL(parser, parse_payload(_, _, _)).WillByDefault(WithArgs<0,2>(Invoke(std::plus<>())));
    ON_CALL(parser, update).WillByDefault(Return());

    EXPECT_CALL(parser, parse_payload(payload_size, _, payload_position)).Times(1);
    EXPECT_CALL(parser, update()).Times(1);

    parser.put(std::move(pusi_packet));
}

TEST(parser_psi, short_non_last_section_parsing){
    mock::parser_psi_t parser;

    const size_t payload_size = 8;
    const size_t payload_position = 8;
    const std::vector<uint8_t> payload{ 0x00, 0x00, 0xb0, 0x0d, 0x41, 0x57, 0xc1, 0x01, 0x02, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    static const bool pusi_flag = true;
    std::unique_ptr<challenge::ts_packet_t> pusi_packet(new challenge::ts_packet_t(0, pusi_flag, payload));

    ON_CALL(parser, parse_payload(_, _, _)).WillByDefault(WithArgs<0,2>(Invoke(std::plus<>())));
    ON_CALL(parser, update).WillByDefault(Return());

    EXPECT_CALL(parser, parse_payload(payload_size, _, payload_position)).Times(1);
    EXPECT_CALL(parser, update()).Times(0);

    parser.put(std::move(pusi_packet));
}

TEST(parser_psi, long_section_parsing){
    mock::parser_psi_t parser;

    const size_t payload_size = 16;
    const size_t payload_position = 8;
    const std::vector<uint8_t> payload_part_1{ 0x00, 0x00, 0xb0, 0x0d + 8, 0x41, 0x57, 0xc1, 0x01, 0x01, 0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    const std::vector<uint8_t> payload_part_2{                                                           0x41, 0x57, 0xe0, 0x20, 0x9d, 0xe6, 0x30, 0x91};
    static const bool pusi_flag = true;
    std::unique_ptr<challenge::ts_packet_t> pusi_packet(new challenge::ts_packet_t(0, pusi_flag, payload_part_1));
    std::unique_ptr<challenge::ts_packet_t> regular_packet(new challenge::ts_packet_t(0, !pusi_flag, payload_part_2));

    ON_CALL(parser, parse_payload(_, _, _)).WillByDefault(WithArgs<0,2>(Invoke(std::plus<>())));
    ON_CALL(parser, update).WillByDefault(Return());

    EXPECT_CALL(parser, parse_payload(payload_size, _, payload_position)).WillOnce(Return(payload_size + payload_position));
    EXPECT_CALL(parser, update()).WillOnce(Return());

    parser.put(std::move(pusi_packet));
    parser.put(std::move(regular_packet));
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
    std::unique_ptr<challenge::ts_packet_t> first_packet(new challenge::ts_packet_t(0, pusi_flag, payload_part_1));
    std::unique_ptr<challenge::ts_packet_t> second_packet(new challenge::ts_packet_t(0, pusi_flag, payload_part_2));

    ON_CALL(parser, parse_payload(_, _, _)).WillByDefault(WithArgs<0,2>(Invoke(std::plus<>())));
    ON_CALL(parser, update).WillByDefault(Return());

    EXPECT_CALL(parser, parse_payload(payload_size_section_1, _, payload_position_section_1)).Times(1);
    EXPECT_CALL(parser, parse_payload(payload_size_section_2, _, payload_position_section_2)).Times(1);
    EXPECT_CALL(parser, update()).Times(2);

    parser.put(std::move(first_packet));
    parser.put(std::move(second_packet));
}
