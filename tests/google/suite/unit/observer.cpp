// LRUCache observers test suite.
//
// Coverage:
// - size(), empty(), capacity() reflect current cache state
// - hitCount(), missCount(), hitRate() track get() outcomes correctly
// - resetStats() zeroes hitCount()/missCount()
// - mostRecentKey()/leastRecentKey() on empty and populated caches

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies size(), empty(), and capacity() report correct values.
TEST(Observer, SizeEmptyCapacity) {
    LRUCache<int, std::string> cache(4);
    EXPECT_EQ(cache.capacity(), 4u);
    EXPECT_EQ(cache.size(), 0u);
    EXPECT_TRUE(cache.empty());

    cache.put(1, "a");
    EXPECT_EQ(cache.size(), 1u);
    EXPECT_FALSE(cache.empty());
}

// Verifies hitCount()/missCount() track get() outcomes.
TEST(Observer, HitAndMissCounts) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.get(1); // hit
    (void)cache.get(2); // miss
    (void)cache.get(1); // hit

    EXPECT_EQ(cache.hitCount(), 2u);
    EXPECT_EQ(cache.missCount(), 1u);
}

// Verifies hitRate() computes the correct percentage, and returns 0 with no lookups.
TEST(Observer, HitRatePercentage) {
    LRUCache<int, std::string> cache(4);
    EXPECT_DOUBLE_EQ(cache.hitRate(), 0.0); // no lookups yet

    cache.put(1, "a");
    (void)cache.get(1); // hit
    (void)cache.get(2); // miss
    (void)cache.get(3); // miss
    (void)cache.get(1); // hit

    EXPECT_DOUBLE_EQ(cache.hitRate(), 50.0); // 2 hits / 4 lookups
}

// Verifies resetStats() zeroes both counters.
TEST(Observer, ResetStatsZeroesCounters) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    (void)cache.get(1);
    (void)cache.get(2);

    cache.resetStats();
    EXPECT_EQ(cache.hitCount(), 0u);
    EXPECT_EQ(cache.missCount(), 0u);
}

// Verifies mostRecentKey()/leastRecentKey() on an empty cache return nullptr.
TEST(Observer, EmptyCacheRecencyPointersAreNull) {
    LRUCache<int, std::string> cache(4);
    EXPECT_EQ(cache.mostRecentKey(), nullptr);
    EXPECT_EQ(cache.leastRecentKey(), nullptr);
}

// Verifies mostRecentKey()/leastRecentKey() on a populated cache.
TEST(Observer, PopulatedCacheRecencyPointers) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    const int* mru = cache.mostRecentKey();
    const int* lru = cache.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 3);
    EXPECT_EQ(*lru, 1);
}
