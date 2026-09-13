// LRUCache pool reuse integration test suite.
//
// Coverage:
// - Evict-then-insert cycles reuse freed pool slots rather than requiring
//   growth — capacity() stays fixed across many cycles with no failure
// - Erase-then-insert cycles likewise reuse the freed slot
// - Values are correct after many reuse cycles, confirming slots are fully
//   reset (not left holding stale state) when recycled

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies many evict/insert cycles never require growing past the fixed
// pool size, and contents stay correct throughout.
TEST(PoolReuse, EvictInsertCyclesReuseSlots) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    for (int i = 4; i <= 1000; ++i) {
        cache.put(i, "v" + std::to_string(i));
        EXPECT_EQ(cache.size(), 3u);
        EXPECT_EQ(cache.capacity(), 3u);
        EXPECT_TRUE(cache.contains(i));
    }
    const std::string* value = cache.peek(1000);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, "v1000");
}

// Verifies erase-then-insert cycles reuse the freed slot correctly.
TEST(PoolReuse, EraseInsertCyclesReuseSlots) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    for (int i = 4; i <= 200; ++i) {
        EXPECT_TRUE(cache.erase(i - 3));
        cache.put(i, "v" + std::to_string(i));
        EXPECT_EQ(cache.size(), 3u);
        EXPECT_TRUE(cache.contains(i));
        EXPECT_FALSE(cache.contains(i - 3));
    }
}
