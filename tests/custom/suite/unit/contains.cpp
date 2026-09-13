// LRUCache contains() test suite.
//
// Coverage:
// - Existing key returns true
// - Missing key returns false
// - Does not change recency order or hit/miss counters

#include <support/framework.h>

using namespace CachePro;

// Verifies an existing key is reported as present.
static void existing_key_returns_true() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    CHK(cache.contains(1));
}

// Verifies a missing key is reported as absent.
static void missing_key_returns_false() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    CHK(!cache.contains(2));
}

// Verifies contains() does not disturb recency order or statistics.
static void contains_has_no_side_effects() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(*cache.mostRecentKey() == 2);

    (void)cache.contains(1);
    CHK(*cache.mostRecentKey() == 2); // unchanged
    CHK(cache.hitCount() == 0);
    CHK(cache.missCount() == 0);
}

// Executes all contains() test cases.
static void run_tests() {
    RUN(existing_key_returns_true);
    RUN(missing_key_returns_false);
    RUN(contains_has_no_side_effects);
}

REGISTER_TEST_SUITE();
