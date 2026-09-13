// LRUCache resize() test suite.
//
// Coverage:
// - Growing preserves all existing entries and raises capacity()
// - Shrinking to a size still >= the live entry count preserves everything
// - Shrinking below the live entry count evicts least-recently-used entries
//   first, keeping the most-recently-used ones
// - resize(0) throws std::invalid_argument

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using namespace CachePro;

// Verifies growing the cache preserves existing entries and raises capacity.
TEST(Resize, GrowPreservesEntries) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.resize(4);
    EXPECT_EQ(cache.capacity(), 4u);
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));

    cache.put(3, "c");
    cache.put(4, "d");
    EXPECT_EQ(cache.size(), 4u);
    EXPECT_TRUE(cache.contains(1)); // no eviction needed, room for all four
}

// Verifies shrinking to a size still covering all live entries keeps them all.
TEST(Resize, ShrinkAboveLiveCountPreservesEntries) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.resize(3);
    EXPECT_EQ(cache.capacity(), 3u);
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
}

// Verifies shrinking below the live entry count evicts the least-recently-used
// entries first, keeping the most-recently-used ones.
TEST(Resize, ShrinkBelowLiveCountEvictsLru) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    cache.resize(2);
    EXPECT_EQ(cache.capacity(), 2u);
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_FALSE(cache.contains(1)); // least-recently-used, evicted
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
}

// Verifies resize(0) is rejected.
TEST(Resize, ResizeToZeroThrows) {
    LRUCache<int, std::string> cache(4);
    ASSERT_THROW(cache.resize(0), std::invalid_argument);
}
