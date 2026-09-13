// LRUCache reserve() test suite.
//
// Coverage:
// - reserve() is a documented no-op: capacity(), size(), and existing
//   entries are unaffected regardless of the argument passed in

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies reserve() does not change capacity or size.
TEST(Reserve, ReserveDoesNotChangeCapacityOrSize) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    cache.reserve(100);
    EXPECT_EQ(cache.capacity(), 4u);
    EXPECT_EQ(cache.size(), 1u);
}

// Verifies reserve() leaves existing entries untouched.
TEST(Reserve, ReserveDoesNotDisturbEntries) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.reserve(0);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    const std::string* v1 = cache.peek(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(*v1, "a");
}
