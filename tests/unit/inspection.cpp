// LRUCache order inspection test suite.
//
// Coverage:
// - keys() returns entries in most- to least-recently-used order
// - keys() on an empty cache returns an empty vector
// - keys() reflects promotion order
// - mostRecentKey() reflects the last-touched entry
// - leastRecentKey() reflects the next eviction candidate
// - mostRecentKey() returns nullptr when the cache is empty
// - leastRecentKey() returns nullptr when the cache is empty

#include <common/framework.h>

using namespace CachePro;

// Verifies that keys() returns entries from most- to least-recently-used.
static void keys_returns_mru_to_lru_order() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    std::vector<int> k = c.keys();

    CHK(k.size() == 3);
    CHK(k[0] == 3);
    CHK(k[1] == 2);
    CHK(k[2] == 1);
}

// Verifies that keys() returns an empty vector when the cache is empty.
static void keys_empty_cache() {
    LRUCache<int, int> c(3);

    std::vector<int> k = c.keys();

    CHK(k.empty() == true);
}

// Verifies that keys() reflects promotion after an entry is accessed.
static void keys_reflect_promotion() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);
    (void)c.get(1);

    std::vector<int> k = c.keys();

    CHK(k[0] == 1);
}

// Verifies that mostRecentKey() identifies the last-touched entry.
static void most_recent_key_reflects_latest_touch() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    (void)c.get(1);

    CHK(*c.mostRecentKey() == 1);
}

// Verifies that leastRecentKey() identifies the next eviction candidate.
static void least_recent_key_reflects_eviction_candidate() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    (void)c.get(1);

    CHK(*c.leastRecentKey() == 2);
}

// Verifies that mostRecentKey() returns nullptr when the cache is empty.
static void most_recent_key_null_when_empty() {
    LRUCache<int, int> c(3);

    CHK(c.mostRecentKey() == nullptr);
}

// Verifies that leastRecentKey() returns nullptr when the cache is empty.
static void least_recent_key_null_when_empty() {
    LRUCache<int, int> c(3);

    CHK(c.leastRecentKey() == nullptr);
}

// Executes all order inspection test cases.
static void run_tests() {
    RUN(keys_returns_mru_to_lru_order);
    RUN(keys_empty_cache);
    RUN(keys_reflect_promotion);
    RUN(most_recent_key_reflects_latest_touch);
    RUN(least_recent_key_reflects_eviction_candidate);
    RUN(most_recent_key_null_when_empty);
    RUN(least_recent_key_null_when_empty);
}

REGISTER_TEST_SUITE();