#include "storage.h"

#include <cassert>

void challenge::storage_t::pop_front(size_t amount) {
    assert(amount <= stored_size);
    while (amount != 0) {
        auto &first_entry = sequence.front();
        size_t popped_size = std::min(amount, first_entry.size);
        first_entry.data += popped_size;
        first_entry.size -= popped_size;

        amount -= popped_size;
        stored_size -= popped_size;

        if (sequence.front().size == 0) {
            sequence.pop_front();
        }
    }
}

uint8_t challenge::storage_t::operator[](size_t index) const {
    assert(index < stored_size);
    auto cursor = std::begin(sequence);
    while (index >= cursor->size) {
        size_t skip_size = std::min(index, cursor->size);
        index -= skip_size;
        cursor++;
    }
    return cursor->data[index];
}

size_t challenge::storage_t::size() const {
    return stored_size;
}

void challenge::storage_t::push_back(const stored_entry_t& entry ) {
    sequence.push_back(entry);
    stored_size += entry.size;
}

const challenge::storage_t::stored_entry_t &challenge::storage_t::front() const {
    return sequence.front();
}
