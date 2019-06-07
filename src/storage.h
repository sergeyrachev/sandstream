#pragma once

#include "ts_packet.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <list>

namespace challenge {

    class storage_t {
    public:
        struct stored_entry_t {
            std::shared_ptr<ts_packet_t> packet;
            const uint8_t *data;
            size_t size;
        };

        storage_t();

        void pop_front(size_t amount);
        uint8_t operator[](size_t index) const;
        size_t size() const;

        void push_back(const stored_entry_t& entry);
        const stored_entry_t &front() const;

    private:
        std::list<stored_entry_t> sequence{};
        size_t stored_size{};
    };
}
