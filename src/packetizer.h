#pragma once

#include "source_data.h"
#include "source_packet.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <vector>
#include <list>

namespace challenge{
    class packetizer_t : public challenge::source_data_t, public challenge::source_packet_t{
    public:
        packetizer_t(size_t max_buffer_size = 10 * 1024 * 1024);

        mutable_buffer_t request() override;
        void confirm(mutable_buffer_t filled_buffer) override ;

        bool is_depleted() const override;
        ts_packet_t get() override;

    private:
        void sync();

    private:
        std::vector<uint8_t> data_storage;
        mutable_buffer_t input_buffer;
    };
}
