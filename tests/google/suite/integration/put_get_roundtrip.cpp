// LRUCache put/get roundtrip integration test suite.
//
// Coverage:
// - A basic full-cycle sanity check: putting a batch of entries and
//   reading them all back returns exactly the values that were stored
// - Overwriting a subset of keys and reading the whole batch back again
//   reflects the updates without disturbing the untouched entries

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies a batch of put() entries all read back correctly via get().
TEST(PutGetRoundtrip, PutBatchThenGetAllRoundtrips) {
    LRUCache<int, std::string> cache(10);
    for (int i = 0; i < 10; ++i) {
        cache.put(i, "value" + std::to_string(i));
    }

    for (int i = 0; i < 10; ++i) {
        std::string* value = cache.get(i);
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, "value" + std::to_string(i));
    }
    EXPECT_EQ(cache.hitCount(), 10u);
    EXPECT_EQ(cache.missCount(), 0u);
}

// Verifies overwriting a subset of keys updates only those entries.
TEST(PutGetRoundtrip, OverwriteSubsetThenRoundtrip) {
    LRUCache<int, std::string> cache(5);
    for (int i = 0; i < 5; ++i) {
        cache.put(i, "old" + std::to_string(i));
    }

    cache.put(1, "new1");
    cache.put(3, "new3");

    std::string* v0 = cache.get(0);
    std::string* v1 = cache.get(1);
    std::string* v2 = cache.get(2);
    std::string* v3 = cache.get(3);
    std::string* v4 = cache.get(4);
    ASSERT_NE(v0, nullptr);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    ASSERT_NE(v3, nullptr);
    ASSERT_NE(v4, nullptr);
    EXPECT_EQ(*v0, "old0");
    EXPECT_EQ(*v1, "new1");
    EXPECT_EQ(*v2, "old2");
    EXPECT_EQ(*v3, "new3");
    EXPECT_EQ(*v4, "old4");
    EXPECT_EQ(cache.size(), 5u);
}
