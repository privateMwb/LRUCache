// LRUCache moveToFront() no-op regression test suite.
//
// Coverage:
// - moveToFront() early-returns when the node is already the
//   most-recently-used entry (head_.next == node), skipping the
//   unlink/relink. This suite pins down that the early return leaves the
//   list links completely intact — no self-linking corruption, no lost
//   neighbors — and that later, real moves still work correctly afterward.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace CachePro;

// Verifies repeatedly touching the already-MRU entry doesn't disturb list order.
TEST(AlreadyFrontNoop, RepeatedTouchOfFrontEntryIsNoop) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(3); // already MRU — exercises the early-return branch
    (void)cache.get(3);
    (void)cache.get(3);

    const int* mru = cache.mostRecentKey();
    const int* lru = cache.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 3);
    EXPECT_EQ(*lru, 1);

    std::vector<int> keys = cache.keys();
    ASSERT_EQ(keys.size(), 3u);
    EXPECT_EQ(keys[0], 3);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 1);
}

// Verifies a real move still works correctly right after the no-op branch fires.
TEST(AlreadyFrontNoop, NoopTouchDoesNotBreakSubsequentRealMove) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(3); // no-op branch
    (void)cache.get(1); // real move: 1 becomes MRU

    std::vector<int> keys = cache.keys();
    ASSERT_EQ(keys.size(), 3u);
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 3);
    EXPECT_EQ(keys[2], 2);
}

// Verifies the no-op branch on a single-entry cache leaves it fully intact.
TEST(AlreadyFrontNoop, NoopTouchOnSingleEntryCache) {
    LRUCache<int, std::string> cache(1);
    cache.put(1, "a");

    (void)cache.get(1);
    (void)cache.get(1);

    const int* mru = cache.mostRecentKey();
    const int* lru = cache.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 1);
    EXPECT_EQ(*lru, 1);
    EXPECT_EQ(cache.size(), 1u);
}
