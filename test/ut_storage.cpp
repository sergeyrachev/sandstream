#include "gtest/gtest.h"

#include "storage.h"

#include <vector>

using challenge::storage_t;

TEST(storage, push_pop){
    storage_t storage;
    std::vector<uint8_t> payload{0x01, 0x02, 0x03};
    storage.push_back({nullptr, payload.data(), payload.size()});

    ASSERT_EQ(3, storage.size());
    ASSERT_EQ(0x02, storage[1]);

    storage.pop_front(1);
    ASSERT_EQ(2, storage.size());
    ASSERT_EQ(0x03, storage[1]);

    storage.push_back({nullptr, payload.data(), payload.size()});
    ASSERT_EQ(5, storage.size());
    ASSERT_EQ(0x03, storage[1]);

    ASSERT_EQ( 2, storage.front().size);
    ASSERT_EQ( 0x02, storage.front().data[0]);

    storage.pop_front(2);
    ASSERT_EQ( 3, storage.front().size);
    ASSERT_EQ( 0x01, storage.front().data[0]);
}



