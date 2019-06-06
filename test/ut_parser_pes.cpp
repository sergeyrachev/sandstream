#include "gtest/gtest.h"

#include "parser_pes.h"

#include "mock_callback_es.h"

namespace uncovered{
    class parser_pes_t : public challenge::parser_pes_t{
    public:
        using challenge::parser_pes_t::parser_pes_t;
        using challenge::parser_pes_t::try_parse;
    };
}

using uncovered::parser_pes_t;
using challenge::storage_t;

TEST(parser_pes, parse_header_full_header){

    storage_t storage;
    const size_t pes_header_data_length = 14;
    const std::vector<uint8_t> payload{0x00, 0x00, 0x01, 0x0e0, 0x00, 0x00, 0x84, 0x80, 0x05, 0x21, 0x00, 0x37, 0x77, 0x41, 0x00, 0x00, 0x00, 0x01, 0x09};

    storage.push_back({nullptr, payload.data(), payload.size()});
    const size_t initial_position = 0;
    auto result = parser_pes_t::try_parse(storage, initial_position);

    ASSERT_TRUE(result.is_success);
    ASSERT_EQ(pes_header_data_length, result.consumed_bytes);
}

TEST(parser_pes, parse_header_partial_data){

    const size_t pes_header_data_length = 14;
    storage_t storage;
    const std::vector<uint8_t> payload_part_1 = {0x00, 0x00, 0x01 };
    const std::vector<uint8_t> payload_part_2 = {0xe0, 0x00};
    const std::vector<uint8_t> payload_part_3 = {0x00, 0x84, 0x80, 0x05, 0x21, 0x00, 0x37, 0x77, 0x41, 0x00, 0x00, 0x00, 0x01, 0x09};

    storage.push_back({nullptr, payload_part_1.data(), payload_part_1.size()});
    const size_t initial_position = 0;
    auto result = parser_pes_t::try_parse(storage, initial_position);

    ASSERT_FALSE(result.is_success);
    ASSERT_EQ(0, result.consumed_bytes);

    storage.push_back({nullptr, payload_part_2.data(), payload_part_2.size()});
    result = parser_pes_t::try_parse(storage, initial_position);
    ASSERT_FALSE(result.is_success);
    ASSERT_EQ(0, result.consumed_bytes);

    storage.push_back({nullptr, payload_part_3.data(), payload_part_3.size()});
    result = parser_pes_t::try_parse(storage, initial_position);
    ASSERT_TRUE(result.is_success);
    ASSERT_EQ(pes_header_data_length, result.consumed_bytes);
}

TEST(parser_pes, general_parsing){
    std::unique_ptr<mock::callback_es_t> callback(new mock::callback_es_t);
    ON_CALL(*callback, on_data(_, _)).WillByDefault(Return());

    const size_t es_payload_size_1 = 5;
    const size_t es_payload_size_2 = 16;
    const std::vector<uint8_t> payload_with_header{0x00, 0x00, 0x01, 0x0e0, 0x00, 0x00, 0x84, 0x80, 0x05, 0x21, 0x00, 0x37, 0x77, 0x41, 0x00, 0x00, 0x00, 0x01, 0x09};
    const std::vector<uint8_t> payload{0x0e0, 0x00, 0x00, 0x84, 0x80, 0x05, 0x21, 0x00, 0x37, 0x77, 0x41, 0x00, 0x00, 0x00, 0x01, 0x09};
    const bool pusi_flag = true;

    EXPECT_CALL(*callback, on_data(_, es_payload_size_1)).Times(2);
    EXPECT_CALL(*callback, on_data(_, es_payload_size_2)).Times(1);

    parser_pes_t parser(std::move(callback));

    parser.put(std::unique_ptr<challenge::ts_packet_t>{new challenge::ts_packet_t(0, pusi_flag, payload_with_header )});
    parser.put(std::unique_ptr<challenge::ts_packet_t>{new challenge::ts_packet_t(0, !pusi_flag, payload )});
    parser.put(std::unique_ptr<challenge::ts_packet_t>{new challenge::ts_packet_t(0, pusi_flag, payload_with_header )});
}


