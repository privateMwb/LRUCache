// LRUCache get() test suite.
//
// Coverage:
// - Hit path returns a pointer to the stored value and marks the entry
//   most-recently-used
// - Hit path increments hitCount()
// - Miss path returns nullptr and leaves the cache unchanged
// - Miss path increments missCount()

#include <support/framework.h>

using namespace CachePro;

// Verifies a hit returns the correct value.
static void hit_returns_value() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    std::string* value = cache.get(1);
    CHK(value != nullptr);
    CHK(*value == "a");
}

// Verifies a hit moves the entry to most-recently-used.
static void hit_updates_recency() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(*cache.mostRecentKey() == 2);

    (void)cache.get(1);
    CHK(*cache.mostRecentKey() == 1);
}

// Verifies a hit increments the hit counter.
static void hit_increments_hit_counter() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.get(1);
    CHK(cache.hitCount() == 1);
    CHK(cache.missCount() == 0);
}

// Verifies a miss returns nullptr.
static void miss_returns_nullptr() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.get(1) == nullptr);
}

// Verifies a miss increments the miss counter and leaves the cache unchanged.
static void miss_increments_miss_counter() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.get(2);
    CHK(cache.missCount() == 1);
    CHK(cache.hitCount() == 0);
    CHK(cache.size() == 1);
}

// Executes all get() test cases.
static void run_tests() {
    RUN(hit_returns_value);
    RUN(hit_updates_recency);
    RUN(hit_increments_hit_counter);
    RUN(miss_returns_nullptr);
    RUN(miss_increments_miss_counter);
}

REGISTER_TEST_SUITE();
