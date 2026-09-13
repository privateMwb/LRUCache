// LRUCache destruction test suite.
//
// Coverage:
// - Destroying a populated cache invokes each stored value's destructor
//   exactly once, with no leaks
// - Eviction destroys the evicted value immediately, not just at cache
//   destruction time
// - erase() destroys the removed value immediately
// - clear() destroys every stored value immediately

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

using namespace CachePro;

namespace {

// Tracks live instances via construction/destruction, to confirm the cache
// never leaks a value or destroys one twice.
struct Tracker {
    static int liveCount;

    Tracker() {
        ++liveCount;
    }
    Tracker(const Tracker&) {
        ++liveCount;
    }
    Tracker(Tracker&&) noexcept {
        ++liveCount;
    }
    Tracker& operator=(const Tracker&) = default;
    Tracker& operator=(Tracker&&) noexcept = default;
    ~Tracker() {
        --liveCount;
    }
};

int Tracker::liveCount = 0;

} // namespace

// Verifies destroying a populated cache destroys every stored value exactly once.
TEST(Destruction, DestroyingCacheDestroysAllValues) {
    Tracker::liveCount = 0;
    {
        LRUCache<int, Tracker> cache(3);
        cache.put(1, Tracker());
        cache.put(2, Tracker());
        cache.put(3, Tracker());
        EXPECT_EQ(Tracker::liveCount, 3);
    }
    EXPECT_EQ(Tracker::liveCount, 0);
}

// Verifies eviction destroys the evicted value immediately.
TEST(Destruction, EvictionDestroysValueImmediately) {
    Tracker::liveCount = 0;
    LRUCache<int, Tracker> cache(2);
    cache.put(1, Tracker());
    cache.put(2, Tracker());
    EXPECT_EQ(Tracker::liveCount, 2);

    cache.put(3, Tracker());          // evicts key 1
    EXPECT_EQ(Tracker::liveCount, 2); // one destroyed, one constructed
}

// Verifies erase() destroys the removed value immediately.
TEST(Destruction, EraseDestroysValueImmediately) {
    Tracker::liveCount = 0;
    LRUCache<int, Tracker> cache(3);
    cache.put(1, Tracker());
    cache.put(2, Tracker());
    EXPECT_EQ(Tracker::liveCount, 2);

    (void)cache.erase(1);
    EXPECT_EQ(Tracker::liveCount, 1);
}

// Verifies clear() destroys every stored value immediately.
TEST(Destruction, ClearDestroysAllValues) {
    Tracker::liveCount = 0;
    LRUCache<int, Tracker> cache(3);
    cache.put(1, Tracker());
    cache.put(2, Tracker());
    cache.clear();
    EXPECT_EQ(Tracker::liveCount, 0);
}
