#include "gtest/gtest.h"

#include "ts_packet.h"

using challenge::ts_packet_t;

TEST(ts_packet, parse_itself_no_adaptation_field_no_pusi){
    const size_t ts_packet_header_length = 4;

    const std::array<uint8_t, ts_packet_t::ts_packet_size> content = {0x47, 0x00, 0x21, 0x11, 0x01 };
    ts_packet_t packet(content.data(), content.size());

    ASSERT_EQ(0x21, packet.pid);
    ASSERT_FALSE(packet.payload_unit_start_indicator);
    ASSERT_EQ(0x01, packet.payload.data[0]);
}

TEST(ts_packet, parse_itself_with_adaptation_field_and_pusi){
    const size_t ts_packet_header_length = 4;
    const size_t adaptation_field_occupied_length = 8;

    std::vector<uint8_t> expected_payload(ts_packet_t::ts_packet_size - ts_packet_header_length - adaptation_field_occupied_length);
    expected_payload[0] = 0xe0;

    const std::array<uint8_t, ts_packet_t::ts_packet_size> content = {0x47, 0x40, 0x21, 0x30, 0x07, 0x50, 0x00, 0x06, 0xc7, 0xd7, 0x7e, 0x00, 0xe0 };
    ts_packet_t packet(content.data(), content.size());

    ASSERT_EQ(0x21, packet.pid);
    ASSERT_TRUE(packet.payload_unit_start_indicator);
    ASSERT_EQ(0xe0, packet.payload.data[0]);
}
