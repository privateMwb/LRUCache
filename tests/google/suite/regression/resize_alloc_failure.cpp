// LRUCache resize() allocation failure regression test suite.
//
// Coverage:
// - resize() allocates the new table (`new Slot[newTableCapacity]`) BEFORE
//   calling rebuildPool()/rebuildTable(). If that allocation throws, the
//   cache must be left completely untouched — old pool, old table, and all
//   existing entries still valid and usable.
// - Regression guard: an earlier ordering that freed the old pool before
//   allocating the new table would leave `table_` referencing freed
//   addresses on a failed allocation (use-after-free on the very next
//   operation).

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <limits>
#include <new>
#include <string>

using namespace CachePro;

// Verifies a resize() whose table allocation fails leaves the cache in its
// original, fully usable state.
TEST(ResizeAllocFailure, ResizeAllocFailureLeavesCacheUnchanged) {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");

    ASSERT_THROW(cache.resize(std::numeric_limits<std::size_t>::max() / 2), std::bad_alloc);

    // Nothing should have been committed: original capacity, size, and
    // contents must all still be intact.
    EXPECT_EQ(cache.capacity(), 3u);
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    const std::string* v1 = cache.peek(1);
    const std::string* v2 = cache.peek(2);
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    EXPECT_EQ(*v1, "a");
    EXPECT_EQ(*v2, "b");
}

// Verifies the cache remains fully functional (no use-after-free) after a
// failed resize() — the real regression this guards against.
TEST(ResizeAllocFailure, CacheUsableAfterResizeAllocFailure) {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");

    try {
        cache.resize(std::numeric_limits<std::size_t>::max() / 2);
    } catch (const std::bad_alloc&) {
        // expected
    }

    // These would be use-after-free if the old pool/table had already
    // been freed before the failed allocation.
    cache.put(2, "b");
    EXPECT_EQ(cache.size(), 2u);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));

    EXPECT_TRUE(cache.erase(1));
    EXPECT_EQ(cache.size(), 1u);
}
