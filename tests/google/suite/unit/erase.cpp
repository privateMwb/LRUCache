// LRUCache erase() test suite.
//
// Coverage:
// - Existing key: returns true, removes the entry, decrements size()
// - Missing key: returns false, leaves the cache unchanged
// - Post-erase state: capacity() is unchanged and the freed slot is
//   reusable by a subsequent put()

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <string>

using namespace CachePro;

// Verifies erasing an existing key succeeds and removes it.
TEST(Erase, ExistingKeyErased) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    EXPECT_TRUE(cache.erase(1));
    EXPECT_FALSE(cache.contains(1));
    EXPECT_EQ(cache.size(), 1u);
}

// Verifies erasing a missing key fails and changes nothing.
TEST(Erase, MissingKeyNotErased) {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    EXPECT_FALSE(cache.erase(2));
    EXPECT_EQ(cache.size(), 1u);
    EXPECT_TRUE(cache.contains(1));
}

// Verifies capacity is unaffected by erase(), and the freed slot can be reused.
TEST(Erase, PostEraseStateAndSlotReuse) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    EXPECT_TRUE(cache.erase(1));
    EXPECT_EQ(cache.capacity(), 2u);

    cache.put(3, "c"); // should reuse the freed slot, not evict
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
}
