// LRUCache update-skips-eviction regression test suite.
//
// Coverage:
// - put()/emplace() on a key that already exists must take the update
//   path even when the cache is at full capacity — it must NOT evict
//   another entry to make room, since no new slot is needed.
// - Every other entry present before the update must still be present
//   after it, with size() unchanged.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies put() on an existing key at full capacity updates in place
// without evicting anything else.
TEST(UpdateSkipsEviction, PutUpdateAtCapacityDoesNotEvict) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    EXPECT_EQ(cache.size(), 2u);

    cache.put(1, "updated");
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2)); // must not have been evicted
    const std::string* v1 = cache.peek(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(*v1, "updated");
}

// Verifies emplace() on an existing key at full capacity updates in place
// without evicting anything else.
TEST(UpdateSkipsEviction, EmplaceUpdateAtCapacityDoesNotEvict) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    EXPECT_EQ(cache.size(), 2u);

    cache.emplace(1, "updated");
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(2)); // must not have been evicted
    const std::string* v1 = cache.peek(1);
    ASSERT_NE(v1, nullptr);
    EXPECT_EQ(*v1, "updated");
}

// Verifies repeatedly updating the same key at full capacity never
// shrinks the live set below capacity via spurious eviction.
TEST(UpdateSkipsEviction, RepeatedUpdatesAtCapacityPreserveAllEntries) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    for (int i = 0; i < 20; ++i) {
        cache.put(2, "v" + std::to_string(i));
        EXPECT_EQ(cache.size(), 3u);
        EXPECT_TRUE(cache.contains(1));
        EXPECT_TRUE(cache.contains(2));
        EXPECT_TRUE(cache.contains(3));
    }
}
