#include "gtest/gtest.h"

#include "packetizer.h"

using challenge::ts_packet_t;

namespace fake{
    class packetizer_t : public challenge::packetizer_t{
    public:
        void put( const uint8_t* p, size_t size){
            auto buffer = request();

            memcpy(buffer.data, p, size);
            buffer.size = size;

            confirm(buffer);
        }
    };
}

using fake::packetizer_t;

TEST(packetizer_t, aligned_packets){
    const std::array<uint8_t, challenge::ts_packet_t::ts_packet_size> single_packet{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    
    packetizer_t packetizer;
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());

    auto packet = packetizer.get();
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());

    packet = packetizer.get();
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet.data(), single_packet.size());
    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();
    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();
    ASSERT_TRUE(packetizer.is_depleted());
}

TEST(packetizer_t, accumulation){
    static const size_t split_size = 4;
    const std::array<uint8_t, split_size> single_packet_head{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    const std::array<uint8_t, ts_packet_t::ts_packet_size - split_size> single_packet_tail{};

    packetizer_t packetizer;
    packetizer.put(single_packet_head.data(), single_packet_head.size());
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    ASSERT_FALSE(packetizer.is_depleted());

    auto packet = packetizer.get();
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet_head.data(), single_packet_head.size());
    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    packetizer.put(single_packet_head.data(), single_packet_head.size());
    ASSERT_FALSE(packetizer.is_depleted());

    packet = packetizer.get();
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    ASSERT_FALSE(packetizer.is_depleted());
}

TEST(packetizer_t, lost_sync){
    static const size_t split_size = 4;
    const std::array<uint8_t, ts_packet_t::ts_packet_size> single_packet{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    const std::array<uint8_t, split_size> single_packet_head{ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};
    const std::array<uint8_t, ts_packet_t::ts_packet_size - split_size> single_packet_tail{};
    const std::array<uint8_t, ts_packet_t::ts_packet_size> garbage{};

    packetizer_t packetizer;
    packetizer.put(garbage.data(), garbage.size());
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(garbage.data(), garbage.size());
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());
    auto packet = packetizer.get();

    packetizer.put(garbage.data(), garbage.size());
    ASSERT_TRUE(packetizer.is_depleted());

    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();


    packetizer.put(single_packet_head.data(), single_packet_head.size());
    ASSERT_TRUE(packetizer.is_depleted());


    packetizer.put(garbage.data(), garbage.size());
    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();

    packetizer.put(single_packet_head.data(), single_packet_head.size());
    packetizer.put(single_packet_tail.data(), single_packet_tail.size());
    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();

    ASSERT_TRUE(packetizer.is_depleted());
}

TEST(packetizer_t, dvb_atsc_packets){
    static const size_t atsc_prefix_length = 4;
    const std::array<uint8_t, ts_packet_t::ts_packet_size + atsc_prefix_length> single_packet{0, 1, 2, 3, ts_packet_t::ts_sync_byte, 0x01, 0x02, 0x03};

    packetizer_t packetizer;
    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());
    auto packet = packetizer.get();

    packetizer.put(single_packet.data(), single_packet.size());
    packetizer.put(single_packet.data(), single_packet.size());
    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();

    ASSERT_FALSE(packetizer.is_depleted());
    packet = packetizer.get();

    ASSERT_TRUE(packetizer.is_depleted());
}



