// LRUCache erase() wraparound regression test suite.
//
// Coverage:
// - eraseSlot()'s backward-shift deletion must stay correct when the probe
//   cluster it's compacting wraps past the end of the table back to index 0
//   — the distance calculations (`distToHole`, `distToProbe`) use modular
//   arithmetic specifically to handle this, and this suite pins it down.
// - Uses a colliding Hash functor to force every key into the same home
//   slot near the end of the table, guaranteeing the resulting cluster
//   spans the wraparound boundary.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <string>

using namespace CachePro;

namespace {

// Forces every key to the same home slot, so linear probing is guaranteed
// to build a cluster that wraps from the end of the table back to index 0.
struct CollidingHash {
    std::size_t operator()(int) const noexcept {
        return 7;
    }
};

using WrapCache = LRUCache<int, std::string, CollidingHash>;

} // namespace

// Verifies erasing an entry in the wrapped portion of a cluster leaves the
// rest of the cluster fully reachable.
TEST(EraseSlotWraparound, EraseMiddleOfWrappedClusterStaysReachable) {
    // capacity 3 -> table size 8 (tableSizeFor(3) = 8), mask 7.
    // All three keys hash to 7, so they land at slots 7, 0, 1 in order —
    // a cluster that wraps across the table boundary.
    WrapCache cache(3);
    cache.put(1, "a"); // slot 7
    cache.put(2, "b"); // slot 0 (wrapped)
    cache.put(3, "c"); // slot 1 (wrapped)

    EXPECT_TRUE(cache.erase(2)); // erase the middle, wrapped entry

    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
    const std::string* v1 = cache.peek(1);
    const std::string* v3 = cache.peek(3);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v3, nullptr);
    EXPECT_EQ(*v1, "a");
    EXPECT_EQ(*v3, "c");
}

// Verifies erasing the entry at the cluster's true home slot (just before
// the wrap) still leaves the wrapped entries reachable.
TEST(EraseSlotWraparound, EraseHomeSlotBeforeWrapKeepsWrappedEntries) {
    WrapCache cache(3);
    cache.put(1, "a"); // slot 7 (home slot, pre-wrap)
    cache.put(2, "b"); // slot 0 (wrapped)
    cache.put(3, "c"); // slot 1 (wrapped)

    EXPECT_TRUE(cache.erase(1));

    EXPECT_FALSE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
    const std::string* v2 = cache.peek(2);
    const std::string* v3 = cache.peek(3);
    ASSERT_NE(v2, nullptr);
    ASSERT_NE(v3, nullptr);
    EXPECT_EQ(*v2, "b");
    EXPECT_EQ(*v3, "c");
}

// Verifies erasing the last entry in a wrapped cluster leaves the earlier
// (pre-wrap) entries reachable.
TEST(EraseSlotWraparound, EraseEndOfWrappedClusterKeepsEarlierEntries) {
    WrapCache cache(3);
    cache.put(1, "a"); // slot 7
    cache.put(2, "b"); // slot 0
    cache.put(3, "c"); // slot 1

    EXPECT_TRUE(cache.erase(3));

    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_FALSE(cache.contains(3));
}
