// LRUCache full-load guard regression test suite.
//
// Coverage:
// - tableSizeFor() always rounds up to a power of two >= 2*capacity,
//   guaranteeing at least one permanently empty slot no matter how full
//   the cache gets. findSlot()/insertSlot()'s linear probe loops rely on
//   hitting that empty slot to terminate — this suite pins down the
//   tightest case (capacity 1, table size 2) where that guarantee is most
//   load-bearing, since it's the smallest table size the class ever builds.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies a lookup for a missing key terminates correctly at the tightest
// possible load (capacity 1, so the table is always exactly half full).
TEST(FullLoadGuard, FindSlotTerminatesAtCapacityOne) {
    LRUCache<int, std::string> cache(1);
    cache.put(1, "a"); // 1 of 2 table slots occupied

    EXPECT_FALSE(cache.contains(2));
    EXPECT_EQ(cache.get(2), nullptr);
    EXPECT_EQ(cache.peek(2), nullptr);
}

// Verifies many put() cycles at capacity 1 terminate and stay correct —
// each insert always finds the guaranteed empty slot to land in.
TEST(FullLoadGuard, PutCyclesTerminateAtCapacityOne) {
    LRUCache<int, std::string> cache(1);
    for (int i = 0; i < 500; ++i) {
        cache.put(i, "v");
        EXPECT_EQ(cache.size(), 1u);
        EXPECT_TRUE(cache.contains(i));
    }
}

// Verifies erase-then-reinsert cycles at capacity 1 also terminate and
// stay correct, alternating which of the two table slots ends up occupied.
TEST(FullLoadGuard, EraseAndReinsertCyclesTerminateAtCapacityOne) {
    LRUCache<int, std::string> cache(1);
    int currentKey = 1;
    cache.put(currentKey, "v");

    for (int i = 2; i < 200; ++i) {
        EXPECT_TRUE(cache.erase(currentKey));
        cache.put(i, "v");
        EXPECT_TRUE(cache.contains(i));
        EXPECT_EQ(cache.size(), 1u);
        currentKey = i;
    }
}
