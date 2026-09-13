// LRUCache put() test suite.
//
// Coverage:
// - All four put() overloads: copy/copy, copy key + move value, move key +
//   copy value, move/move
// - Insert path adds a new entry as most-recently-used
// - Update path replaces the value in place, leaves size unchanged, and
//   moves the existing entry to most-recently-used

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies put(const K&, const V&) inserts a retrievable entry.
TEST(Put, CopyKeyCopyValue) {
    LRUCache<int, std::string> cache(4);
    int key = 1;
    std::string value = "one";
    cache.put(key, value);
    const std::string* v = cache.peek(1);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(*v, "one");
}

// Verifies put(const K&, V&&) inserts a retrievable entry.
TEST(Put, CopyKeyMoveValue) {
    LRUCache<int, std::string> cache(4);
    int key = 1;
    cache.put(key, std::string("one"));
    const std::string* v = cache.peek(1);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(*v, "one");
}

// Verifies put(K&&, const V&) inserts a retrievable entry.
TEST(Put, MoveKeyCopyValue) {
    LRUCache<int, std::string> cache(4);
    std::string value = "one";
    cache.put(1, value);
    const std::string* v = cache.peek(1);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(*v, "one");
}

// Verifies put(K&&, V&&) inserts a retrievable entry.
TEST(Put, MoveKeyMoveValue) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, std::string("one"));
    const std::string* v = cache.peek(1);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(*v, "one");
}

// Verifies inserting a new key becomes the most-recently-used entry.
TEST(Put, InsertPathBecomesMru) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    const int* mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 2);
}

// Verifies updating an existing key replaces the value, leaves size
// unchanged, and moves the entry to most-recently-used.
TEST(Put, UpdatePathReplacesAndMovesFront) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    EXPECT_EQ(cache.size(), 2u);

    cache.put(1, "updated");
    EXPECT_EQ(cache.size(), 2u);
    const std::string* v1 = cache.peek(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(*v1, "updated");
    const int* mru = cache.mostRecentKey();
    ASSERT_NE(mru, nullptr);
    EXPECT_EQ(*mru, 1);
}
