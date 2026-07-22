// LRUCache observers test suite.
//
// Coverage:
// - size(), empty(), capacity() reflect current cache state
// - hitCount(), missCount(), hitRate() track get() outcomes correctly
// - resetStats() zeroes hitCount()/missCount()
// - mostRecentKey()/leastRecentKey() on empty and populated caches

#include <support/framework.h>

using namespace CachePro;

// Verifies size(), empty(), and capacity() report correct values.
static void size_empty_capacity() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.capacity() == 4);
    CHK(cache.size() == 0);
    CHK(cache.empty());

    cache.put(1, "a");
    CHK(cache.size() == 1);
    CHK(!cache.empty());
}

// Verifies hitCount()/missCount() track get() outcomes.
static void hit_and_miss_counts() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.get(1); // hit
    (void)cache.get(2); // miss
    (void)cache.get(1); // hit

    CHK(cache.hitCount() == 2);
    CHK(cache.missCount() == 1);
}

// Verifies hitRate() computes the correct percentage, and returns 0 with no lookups.
static void hit_rate_percentage() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.hitRate() == 0.0); // no lookups yet

    cache.put(1, "a");
    (void)(void)cache.get(1); // hit
    (void)cache.get(2);       // miss
    (void)cache.get(3);       // miss
    (void)cache.get(1);       // hit

    CHK(cache.hitRate() == 50.0); // 2 hits / 4 lookups
}

// Verifies resetStats() zeroes both counters.
static void reset_stats_zeroes_counters() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    (void)cache.get(1);
    (void)cache.get(2);

    cache.resetStats();
    CHK(cache.hitCount() == 0);
    CHK(cache.missCount() == 0);
}

// Verifies mostRecentKey()/leastRecentKey() on an empty cache return nullptr.
static void empty_cache_recency_pointers_are_null() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.mostRecentKey() == nullptr);
    CHK(cache.leastRecentKey() == nullptr);
}

// Verifies mostRecentKey()/leastRecentKey() on a populated cache.
static void populated_cache_recency_pointers() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    CHK(*cache.mostRecentKey() == 3);
    CHK(*cache.leastRecentKey() == 1);
}

// Executes all observer test cases.
static void run_tests() {
    RUN(size_empty_capacity);
    RUN(hit_and_miss_counts);
    RUN(hit_rate_percentage);
    RUN(reset_stats_zeroes_counters);
    RUN(empty_cache_recency_pointers_are_null);
    RUN(populated_cache_recency_pointers);
}

REGISTER_TEST_SUITE();
