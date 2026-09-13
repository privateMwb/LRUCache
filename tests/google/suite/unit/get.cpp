// LRUCache get() test suite.
//
// Coverage:
// - Hit path returns a pointer to the stored value and marks the entry
//   most-recently-used
// - Hit path increments hitCount()
// - Miss path returns nullptr and leaves the cache unchanged
// - Miss path increments missCount()

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies a hit returns the correct value.
TEST(Get, HitReturnsValue) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    const std::string* value = cache.get(1);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, "a");
}

// Verifies a hit moves the entry to most-recently-used.
TEST(Get, HitUpdatesRecency) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    const int* mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 2);

    (void)cache.get(1);
    mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 1);
}

// Verifies a hit increments the hit counter.
TEST(Get, HitIncrementsHitCounter) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.get(1);
    EXPECT_EQ(cache.hitCount(), 1u);
    EXPECT_EQ(cache.missCount(), 0u);
}

// Verifies a miss returns nullptr.
TEST(Get, MissReturnsNullptr) {
    LRUCache<int, std::string> cache(4);
    EXPECT_EQ(cache.get(1), nullptr);
}

// Verifies a miss increments the miss counter and leaves the cache unchanged.
TEST(Get, MissIncrementsMissCounter) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.get(2);
    EXPECT_EQ(cache.missCount(), 1u);
    EXPECT_EQ(cache.hitCount(), 0u);
    EXPECT_EQ(cache.size(), 1u);
}
