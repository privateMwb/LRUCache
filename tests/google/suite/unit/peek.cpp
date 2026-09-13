// LRUCache peek() test suite.
//
// Coverage:
// - Hit path returns a pointer to the stored value
// - Miss path returns nullptr
// - Neither path changes recency order
// - Neither path affects hitCount()/missCount()

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies a hit returns the correct value.
TEST(Peek, HitReturnsValue) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    const std::string* value = cache.peek(1);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, "a");
}

// Verifies a hit does not change recency order.
TEST(Peek, HitDoesNotUpdateRecency) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    const int* mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 2);

    (void)cache.peek(1);
    mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 2); // unchanged, still 2
}

// Verifies a miss returns nullptr.
TEST(Peek, MissReturnsNullptr) {
    LRUCache<int, std::string> cache(4);
    EXPECT_EQ(cache.peek(1), nullptr);
}

// Verifies peek() never touches the hit/miss counters, on either path.
TEST(Peek, PeekDoesNotAffectStats) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.peek(1);
    (void)cache.peek(2);
    EXPECT_EQ(cache.hitCount(), 0u);
    EXPECT_EQ(cache.missCount(), 0u);
}
