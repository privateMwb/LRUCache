// LRUCache emplace() test suite.
//
// Coverage:
// - Insert path constructs the value in place from the given arguments,
//   evicting the least-recently-used entry first if at capacity
// - Update path replaces the existing value with a freshly constructed one
//   and moves the entry to most-recently-used
// - Both paths return a reference to the (possibly newly inserted) value

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies emplace() on a new key constructs the value in place and returns it.
TEST(Emplace, InsertPathConstructsValue) {
    LRUCache<int, std::string> cache(4);
    std::string& value = cache.emplace(1, 3, 'x'); // std::string(3, 'x') == "xxx"
    EXPECT_EQ(value, "xxx");
    const std::string* v1 = cache.peek(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(*v1, "xxx");
}

// Verifies emplace() evicts the least-recently-used entry when inserting at capacity.
TEST(Emplace, InsertPathEvictsAtCapacity) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    const int* lru = cache.leastRecentKey();
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*lru, 1);

    cache.emplace(3, "c");
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_FALSE(cache.contains(1)); // evicted
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
}

// Verifies emplace() on an existing key replaces the value and moves it to MRU.
TEST(Emplace, UpdatePathReplacesAndMovesFront) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    std::string& value = cache.emplace(1, 3, 'y'); // std::string(3, 'y') == "yyy"
    EXPECT_EQ(value, "yyy");
    const std::string* v1 = cache.peek(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(*v1, "yyy");
    const int* mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 1);
    EXPECT_EQ(cache.size(), 2u);
}
