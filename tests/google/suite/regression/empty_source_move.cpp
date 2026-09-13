// LRUCache empty-source move regression test suite.
//
// Coverage:
// - Move construction/assignment branches on whether the source's list is
//   empty (`other.head_.next == &other.tail_`) versus populated, since an
//   empty list's sentinels point at each other rather than at real nodes
//   that can simply be re-pointed.
// - This suite pins down that moving from an already-empty cache takes
//   that branch correctly: the destination's sentinels end up properly
//   self-linked (not dangling at the source's now-moved-from address),
//   and the destination is fully usable afterward.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>
#include <utility>

using namespace CachePro;

// Verifies move-constructing from an empty source correctly initializes
// the destination's list, confirmed by subsequent put()s working correctly.
TEST(EmptySourceMove, MoveConstructFromEmptySourceInitializesList) {
    LRUCache<int, std::string> source(4);

    LRUCache<int, std::string> dest(std::move(source));
    EXPECT_EQ(dest.mostRecentKey(), nullptr);
    EXPECT_EQ(dest.leastRecentKey(), nullptr);
    EXPECT_TRUE(dest.empty());

    // If the sentinels were left dangling at the source's old addresses
    // rather than correctly self-linked, these would corrupt the list or
    // crash rather than behaving like a normal, freshly built cache.
    dest.put(1, "a");
    dest.put(2, "b");
    const int* mru = dest.mostRecentKey();
    const int* lru = dest.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 2);
    EXPECT_EQ(*lru, 1);
    EXPECT_EQ(dest.size(), 2u);
}

// Verifies move-assigning from an empty source correctly reinitializes the
// destination's list, even when the destination previously held entries.
TEST(EmptySourceMove, MoveAssignFromEmptySourceInitializesList) {
    LRUCache<int, std::string> dest(4);
    dest.put(99, "stale");

    LRUCache<int, std::string> source(3);
    dest = std::move(source);

    EXPECT_EQ(dest.mostRecentKey(), nullptr);
    EXPECT_EQ(dest.leastRecentKey(), nullptr);
    EXPECT_TRUE(dest.empty());
    EXPECT_FALSE(dest.contains(99));

    dest.put(1, "a");
    dest.put(2, "b");
    const int* mru = dest.mostRecentKey();
    const int* lru = dest.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 2);
    EXPECT_EQ(*lru, 1);
}

// Verifies moving from an empty source into an also-empty destination
// still produces a correctly self-linked, usable cache.
TEST(EmptySourceMove, MoveBetweenTwoEmptyCaches) {
    LRUCache<int, std::string> dest(2);
    LRUCache<int, std::string> source(5);

    dest = std::move(source);
    EXPECT_EQ(dest.capacity(), 5u);
    EXPECT_TRUE(dest.empty());

    dest.put(1, "a");
    const int* mru = dest.mostRecentKey();
    const int* lru = dest.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 1);
    EXPECT_EQ(*lru, 1);
}
