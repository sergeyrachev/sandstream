#include "gtest/gtest.h"
#include "masked_two_bytes_value.h"

#include <array>

using collection_t = std::array<uint8_t, 2>;
using challenge::masked_two_bytes_value_t;

TEST(masked_two_bytes_value, non_masked){
    collection_t collection{0xab, 0xcd};

    auto result = masked_two_bytes_value_t(collection.data()).value;
    ASSERT_EQ(0xabcd, result);
}

TEST(masked_two_bytes_value, masked){
    collection_t collection{0xab, 0xcd};

    auto result = masked_two_bytes_value_t(collection.data(), 0x0f).value;
    ASSERT_EQ(0xbcd, result);

    result = masked_two_bytes_value_t(collection.data(), 0xf0).value;
    ASSERT_EQ(0xa0cd, result);
}
