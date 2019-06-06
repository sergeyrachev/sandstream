#include "gtest/gtest.h"

#include "packetizer.h"

using challenge::packetizer_t;
using challenge::ts_packet_t;

TEST(packetizer_t, aligned_packets){
    const std::array<uint8_t, ts_packet_t::ts_packet_size> single_packet{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    
    packetizer_t packetizer;
    auto packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet.data(), single_packet.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet.data(), single_packet.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet.data(), single_packet.size());
    packetizer.put(single_packet.data(), single_packet.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_FALSE(packet);
}

TEST(packetizer_t, accumulation){
    static const size_t split_size = 4;
    const std::array<uint8_t, split_size> single_packet_head{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    const std::array<uint8_t, ts_packet_t::ts_packet_size - split_size> single_packet_tail{};

    packetizer_t packetizer;
    packetizer.put(single_packet_head.data(), single_packet_head.size());
    auto packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);

    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet_head.data(), single_packet_head.size());
    packetizer.put(single_packet_tail.data(), single_packet_tail.size());

    packetizer.put(single_packet_head.data(), single_packet_head.size());

    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);
}

TEST(packetizer_t, lost_sync){
    static const size_t split_size = 4;
    const std::array<uint8_t, ts_packet_t::ts_packet_size> single_packet{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    const std::array<uint8_t, split_size> single_packet_head{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    const std::array<uint8_t, ts_packet_t::ts_packet_size - split_size> single_packet_tail{};
    const std::array<uint8_t, ts_packet_t::ts_packet_size> garbage{};

    packetizer_t packetizer;
    packetizer.put(garbage.data(), garbage.size());

    auto packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(garbage.data(), garbage.size());
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet.data(), single_packet.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);

    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(garbage.data(), garbage.size());
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet.data(), single_packet.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);

    packetizer.put(single_packet_head.data(), single_packet_head.size());
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(garbage.data(), garbage.size());
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet_head.data(), single_packet_head.size());
    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);
}

TEST(packetizer_t, dvb_atsc_packets){
    static const size_t atsc_prefix_length = 4;
    const std::array<uint8_t, ts_packet_t::ts_packet_size + atsc_prefix_length> single_packet{0, 1, 2, 3, ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};

    packetizer_t packetizer;
    packetizer.put(single_packet.data(), single_packet.size());
    auto packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_FALSE(packet);

    packetizer.put(single_packet.data(), single_packet.size());
    packetizer.put(single_packet.data(), single_packet.size());
    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_TRUE(packet);
    packet = packetizer.get();
    ASSERT_FALSE(packet);
}



