// LRUCache shrink_to_fit() test suite.
//
// Coverage:
// - Reclaims unused capacity: after shrink_to_fit(), capacity() == size()
// - No-op when the cache is empty (size() == 0)
// - No-op when the cache is already at full capacity (size() == capacity())
// - All existing entries, values, and recency order survive the shrink
// - The cache remains fully usable afterward (further put()s respect the
//   new, smaller capacity and evict correctly once full)

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies shrink_to_fit() reclaims unused capacity down to the live size.
TEST(ShrinkToFit, ShrinkToFitReclaimsUnusedCapacity) {
    LRUCache<int, std::string> cache(10);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");
    EXPECT_EQ(cache.capacity(), 10u);

    cache.shrink_to_fit();
    EXPECT_EQ(cache.capacity(), 3u);
    EXPECT_EQ(cache.size(), 3u);
}

// Verifies shrink_to_fit() is a no-op on an empty cache.
TEST(ShrinkToFit, ShrinkToFitNoopWhenEmpty) {
    LRUCache<int, std::string> cache(10);

    cache.shrink_to_fit();
    EXPECT_EQ(cache.capacity(), 10u);
    EXPECT_EQ(cache.size(), 0u);
    EXPECT_TRUE(cache.empty());
}

// Verifies shrink_to_fit() is a no-op when already at full capacity.
TEST(ShrinkToFit, ShrinkToFitNoopWhenFull) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    cache.shrink_to_fit();
    EXPECT_EQ(cache.capacity(), 3u);
    EXPECT_EQ(cache.size(), 3u);
}

// Verifies all values and recency order survive the shrink.
TEST(ShrinkToFit, ShrinkToFitPreservesValuesAndRecency) {
    LRUCache<int, std::string> cache(8);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    cache.shrink_to_fit();
    EXPECT_EQ(cache.capacity(), 3u);
    const std::string* v1 = cache.peek(1);
    const std::string* v2 = cache.peek(2);
    const std::string* v3 = cache.peek(3);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    ASSERT_NE(v3, nullptr);
    EXPECT_EQ(*v1, "a");
    EXPECT_EQ(*v2, "b");
    EXPECT_EQ(*v3, "c");
    const int* mru = cache.mostRecentKey();
    const int* lru = cache.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 3);
    EXPECT_EQ(*lru, 1);
}

// Verifies the cache remains fully usable after shrinking, including
// correct eviction once the new, smaller capacity is reached.
TEST(ShrinkToFit, CacheUsableAfterShrinkToFit) {
    LRUCache<int, std::string> cache(8);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.shrink_to_fit();
    EXPECT_EQ(cache.capacity(), 2u);

    cache.put(3, "c"); // now at capacity, no eviction yet
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_FALSE(cache.contains(1)); // 1 was LRU, evicted to make room
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
}
