// LRUCache eviction order integration test suite.
//
// Coverage:
// - Filling to capacity and inserting past it evicts the correct
//   least-recently-used entry end-to-end (list + table + pool acting
//   together, not any single component in isolation)
// - get()-driven recency changes alter which entry is evicted next
// - Repeated eviction cycles keep evicting the correct entry over time

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies inserting past capacity evicts the least-recently-used entry.
TEST(EvictionOrder, InsertPastCapacityEvictsLru) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    cache.put(4, "d"); // evicts 1
    EXPECT_EQ(cache.size(), 3u);
    EXPECT_FALSE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
    EXPECT_TRUE(cache.contains(4));
}

// Verifies a get() on the current LRU entry saves it from the next eviction.
TEST(EvictionOrder, GetReprievesEntryFromEviction) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(1); // 1 becomes MRU: 1, 3, 2
    cache.put(4, "d");  // now evicts 2, not 1

    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
    EXPECT_TRUE(cache.contains(4));
}

// Verifies repeated fill-and-evict cycles keep evicting the correct entry.
TEST(EvictionOrder, RepeatedEvictionCyclesStayCorrect) {
    LRUCache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);

    for (int i = 3; i <= 10; ++i) {
        const int* leastRecent = cache.leastRecentKey();
        ASSERT_NE(leastRecent, nullptr);
        int aboutToEvict = *leastRecent;
        cache.put(i, i);
        EXPECT_FALSE(cache.contains(aboutToEvict));
        EXPECT_TRUE(cache.contains(i));
        EXPECT_EQ(cache.size(), 2u);
    }
}
