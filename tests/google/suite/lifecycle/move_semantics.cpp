// LRUCache move semantics test suite.
//
// Coverage:
// - Move construction: the moved-to cache inherits capacity, size,
//   contents, and recency order
// - Move assignment: replaces the destination's existing contents entirely
// - Self-move-assignment is a safe no-op
// - The moved-from cache is left empty (size() == 0, capacity() == 0) and
//   remains safely destructible and reusable via move-assignment
// - Moving from an already-empty source, by construction or assignment,
//   takes the empty-list branch correctly

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>
#include <utility>

using namespace CachePro;

// Verifies the moved-to cache takes over capacity, size, and contents.
TEST(MoveSemantics, MoveConstructInheritsState) {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");
    source.put(2, "b");

    LRUCache<int, std::string> dest(std::move(source));
    EXPECT_EQ(dest.capacity(), 4u);
    EXPECT_EQ(dest.size(), 2u);
    const std::string* v1 = dest.peek(1);
    const std::string* v2 = dest.peek(2);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    EXPECT_EQ(*v1, "a");
    EXPECT_EQ(*v2, "b");
}

// Verifies the moved-to cache preserves recency order.
TEST(MoveSemantics, MoveConstructPreservesRecency) {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");
    source.put(2, "b");
    source.put(3, "c"); // MRU order: 3, 2, 1

    LRUCache<int, std::string> dest(std::move(source));
    const int* mru = dest.mostRecentKey();
    const int* lru = dest.leastRecentKey();
    ASSERT_NE(mru, nullptr);
    ASSERT_NE(lru, nullptr);
    EXPECT_EQ(*mru, 3);
    EXPECT_EQ(*lru, 1);
}

// Verifies the moved-from cache is left empty after move construction.
TEST(MoveSemantics, MoveConstructLeavesSourceEmpty) {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");

    LRUCache<int, std::string> dest(std::move(source));

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.capacity(), 0u);
    EXPECT_TRUE(source.empty());
}

// Verifies moving from an already-empty cache takes the empty-list branch.
TEST(MoveSemantics, MoveConstructFromEmptySource) {
    LRUCache<int, std::string> source(4);

    LRUCache<int, std::string> dest(std::move(source));
    EXPECT_EQ(dest.size(), 0u);
    EXPECT_EQ(dest.capacity(), 4u);
    EXPECT_TRUE(dest.empty());
}

// Verifies move-assign replaces the destination's prior contents.
TEST(MoveSemantics, MoveAssignReplacesContents) {
    LRUCache<int, std::string> dest(4);
    dest.put(100, "old");

    LRUCache<int, std::string> source(2);
    source.put(1, "a");
    source.put(2, "b");

    dest = std::move(source);
    EXPECT_EQ(dest.capacity(), 2u);
    EXPECT_EQ(dest.size(), 2u);
    EXPECT_FALSE(dest.contains(100)); // prior contents gone
    const std::string* v1 = dest.peek(1);
    const std::string* v2 = dest.peek(2);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    EXPECT_EQ(*v1, "a");
    EXPECT_EQ(*v2, "b");
}

// Verifies self-move-assignment is a safe no-op.
TEST(MoveSemantics, SelfMoveAssignIsSafe) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache = std::move(cache);
    EXPECT_EQ(cache.size(), 2u);
    const std::string* v1 = cache.peek(1);
    const std::string* v2 = cache.peek(2);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    EXPECT_EQ(*v1, "a");
    EXPECT_EQ(*v2, "b");
}

// Verifies the moved-from cache is left empty after move-assignment.
TEST(MoveSemantics, MoveAssignLeavesSourceEmpty) {
    LRUCache<int, std::string> dest(4);
    LRUCache<int, std::string> source(2);
    source.put(1, "a");

    dest = std::move(source);

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.capacity(), 0u);
    EXPECT_TRUE(source.empty());
}

// Verifies move-assigning from an already-empty source takes the empty-list branch.
TEST(MoveSemantics, MoveAssignFromEmptySource) {
    LRUCache<int, std::string> dest(4);
    dest.put(1, "a");

    LRUCache<int, std::string> source(3);

    dest = std::move(source);
    EXPECT_EQ(dest.size(), 0u);
    EXPECT_EQ(dest.capacity(), 3u);
    EXPECT_TRUE(dest.empty());
}

// Verifies a moved-from cache remains safely destructible and can be
// reused by move-assigning a fresh cache into it.
TEST(MoveSemantics, MovedFromCacheReusableViaMoveAssign) {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");

    LRUCache<int, std::string> dest(std::move(source));

    LRUCache<int, std::string> replacement(2);
    replacement.put(9, "z");
    source = std::move(replacement);

    EXPECT_EQ(source.capacity(), 2u);
    const std::string* v9 = source.peek(9);
    ASSERT_NE(v9, nullptr);
    EXPECT_EQ(*v9, "z");
}
