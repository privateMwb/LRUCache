// LRUCache keys() test suite.
//
// Coverage:
// - Returns all keys ordered most-recently-used first
// - Reflects recency changes caused by get()/put()
// - An empty cache returns an empty vector

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace CachePro;

// Verifies keys() returns entries in most-recently-used-first order.
TEST(Keys, KeysOrderedMruFirst) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    std::vector<int> result = cache.keys();
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 3);
    EXPECT_EQ(result[1], 2);
    EXPECT_EQ(result[2], 1);
}

// Verifies keys() reflects recency changes from a get().
TEST(Keys, KeysReflectRecencyUpdates) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(1); // 1 becomes MRU: 1, 3, 2

    std::vector<int> result = cache.keys();
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 3);
    EXPECT_EQ(result[2], 2);
}

// Verifies an empty cache returns an empty key list.
TEST(Keys, EmptyCacheReturnsEmptyKeys) {
    LRUCache<int, std::string> cache(4);
    EXPECT_TRUE(cache.keys().empty());
}
