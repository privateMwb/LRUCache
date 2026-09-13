// LRUCache contains() test suite.
//
// Coverage:
// - Existing key returns true
// - Missing key returns false
// - Does not change recency order or hit/miss counters

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies an existing key is reported as present.
TEST(Contains, ExistingKeyReturnsTrue) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    EXPECT_TRUE(cache.contains(1));
}

// Verifies a missing key is reported as absent.
TEST(Contains, MissingKeyReturnsFalse) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    EXPECT_FALSE(cache.contains(2));
}

// Verifies contains() does not disturb recency order or statistics.
TEST(Contains, ContainsHasNoSideEffects) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    const int* mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 2);

    (void)cache.contains(1);
    mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 2); // unchanged
    EXPECT_EQ(cache.hitCount(), 0u);
    EXPECT_EQ(cache.missCount(), 0u);
}
