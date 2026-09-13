// LRUCache construction test suite.
//
// Coverage:
// - Valid capacity constructs an empty cache with correct capacity()/size()
// - Pool and table are sized for the full requested capacity at construction
//   (all `capacity` distinct inserts succeed with no premature eviction)
// - Zero capacity throws std::invalid_argument
// - Capacity of 1 is a valid edge case and behaves like any other capacity

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using namespace CachePro;

// Verifies a freshly constructed cache reports the requested capacity and starts empty.
TEST(Construction, ValidCapacityConstructsEmpty) {
    LRUCache<int, std::string> cache(4);
    EXPECT_EQ(cache.capacity(), 4u);
    EXPECT_EQ(cache.size(), 0u);
    EXPECT_TRUE(cache.empty());
}

// Verifies the pool and table are allocated to hold the full requested
// capacity up front — inserting exactly `capacity` distinct keys never
// triggers an early eviction.
TEST(Construction, PoolAndTableSizedForFullCapacity) {
    LRUCache<int, std::string> cache(5);
    for (int i = 0; i < 5; ++i) {
        cache.put(i, "v");
    }
    EXPECT_EQ(cache.size(), 5u);
    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(cache.contains(i));
    }
}

// Verifies capacity of 0 is rejected.
TEST(Construction, ZeroCapacityThrows) {
    ASSERT_THROW((LRUCache<int, std::string>(0)), std::invalid_argument);
}

// Verifies capacity of 1 is a valid edge case: single entry fits, second put evicts it.
TEST(Construction, CapacityOneEdgeCase) {
    LRUCache<int, std::string> cache(1);
    cache.put(1, "a");
    EXPECT_EQ(cache.size(), 1u);
    EXPECT_TRUE(cache.contains(1));

    cache.put(2, "b");
    EXPECT_EQ(cache.size(), 1u);
    EXPECT_FALSE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
}
