// LRUCache destruction test suite.
//
// Coverage:
// - Destroying a populated cache invokes each stored value's destructor
//   exactly once, with no leaks
// - Eviction destroys the evicted value immediately, not just at cache
//   destruction time
// - erase() destroys the removed value immediately
// - clear() destroys every stored value immediately

#include <support/framework.h>

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
static void destroying_cache_destroys_all_values() {
    Tracker::liveCount = 0;
    {
        LRUCache<int, Tracker> cache(3);
        cache.put(1, Tracker());
        cache.put(2, Tracker());
        cache.put(3, Tracker());
        CHK(Tracker::liveCount == 3);
    }
    CHK(Tracker::liveCount == 0);
}

// Verifies eviction destroys the evicted value immediately.
static void eviction_destroys_value_immediately() {
    Tracker::liveCount = 0;
    LRUCache<int, Tracker> cache(2);
    cache.put(1, Tracker());
    cache.put(2, Tracker());
    CHK(Tracker::liveCount == 2);

    cache.put(3, Tracker());      // evicts key 1
    CHK(Tracker::liveCount == 2); // one destroyed, one constructed
}

// Verifies erase() destroys the removed value immediately.
static void erase_destroys_value_immediately() {
    Tracker::liveCount = 0;
    LRUCache<int, Tracker> cache(3);
    cache.put(1, Tracker());
    cache.put(2, Tracker());
    CHK(Tracker::liveCount == 2);

    (void)cache.erase(1);
    CHK(Tracker::liveCount == 1);
}

// Verifies clear() destroys every stored value immediately.
static void clear_destroys_all_values() {
    Tracker::liveCount = 0;
    LRUCache<int, Tracker> cache(3);
    cache.put(1, Tracker());
    cache.put(2, Tracker());
    cache.clear();
    CHK(Tracker::liveCount == 0);
}

// Executes all destruction test cases.
static void run_tests() {
    RUN(destroying_cache_destroys_all_values);
    RUN(eviction_destroys_value_immediately);
    RUN(erase_destroys_value_immediately);
    RUN(clear_destroys_all_values);
}

REGISTER_TEST_SUITE();
