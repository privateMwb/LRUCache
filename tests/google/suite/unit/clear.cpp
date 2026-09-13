// LRUCache clear() test suite.
//
// Coverage:
// - All entries are removed: size() drops to 0, empty() becomes true
// - Previously stored keys are no longer found
// - capacity() is unchanged
// - The cache is fully reusable for further put()/get() calls afterward

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies clear() empties the cache.
TEST(Clear, ClearEmptiesCache) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.clear();
    EXPECT_EQ(cache.size(), 0u);
    EXPECT_TRUE(cache.empty());
    EXPECT_FALSE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
}

// Verifies clear() leaves capacity unchanged.
TEST(Clear, ClearPreservesCapacity) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    cache.clear();
    EXPECT_EQ(cache.capacity(), 4u);
}

// Verifies the cache is reusable after clear().
TEST(Clear, CacheReusableAfterClear) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.clear();

    cache.put(2, "b");
    EXPECT_EQ(cache.size(), 1u);
    const std::string* v2 = cache.peek(2);
    ASSERT_NE(v2, nullptr);
    EXPECT_EQ(*v2, "b");
}
