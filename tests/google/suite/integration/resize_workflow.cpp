// LRUCache resize workflow integration test suite.
//
// Coverage:
// - Grow, then shrink, then continue normal use — pool and table stay
//   consistent with each other across both resize directions
// - Recency order and values survive the full grow/shrink/use cycle
// - The cache remains fully functional (put/get/evict) after resizing

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies a grow-then-shrink-then-use workflow behaves correctly end-to-end.
TEST(ResizeWorkflow, GrowThenShrinkThenContinueUse) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");

    // Grow: room for more entries, nothing evicted.
    cache.resize(5);
    cache.put(3, "c");
    cache.put(4, "d");
    cache.put(5, "e");
    EXPECT_EQ(cache.size(), 5u);
    EXPECT_EQ(cache.capacity(), 5u);
    EXPECT_TRUE(cache.contains(1));

    // MRU order right now: 5, 4, 3, 2, 1
    (void)cache.get(1); // 1 becomes MRU: 1, 5, 4, 3, 2

    // Shrink below live count: evicts LRU entries first (2, then 3).
    cache.resize(3);
    EXPECT_EQ(cache.capacity(), 3u);
    EXPECT_EQ(cache.size(), 3u);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(4));
    EXPECT_TRUE(cache.contains(5));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_FALSE(cache.contains(3));

    // Cache remains fully usable: normal put/get/evict still work.
    cache.put(6, "f"); // evicts current LRU
    EXPECT_EQ(cache.size(), 3u);
    EXPECT_TRUE(cache.contains(6));
    const std::string* value = cache.peek(6);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, "f");
}
