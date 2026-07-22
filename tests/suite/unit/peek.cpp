// LRUCache peek() test suite.
//
// Coverage:
// - Hit path returns a pointer to the stored value
// - Miss path returns nullptr
// - Neither path changes recency order
// - Neither path affects hitCount()/missCount()

#include <support/framework.h>

using namespace CachePro;

// Verifies a hit returns the correct value.
static void hit_returns_value() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    const std::string* value = cache.peek(1);
    CHK(value != nullptr);
    CHK(*value == "a");
}

// Verifies a hit does not change recency order.
static void hit_does_not_update_recency() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(*cache.mostRecentKey() == 2);

    (void)cache.peek(1);
    CHK(*cache.mostRecentKey() == 2); // unchanged, still 2
}

// Verifies a miss returns nullptr.
static void miss_returns_nullptr() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.peek(1) == nullptr);
}

// Verifies peek() never touches the hit/miss counters, on either path.
static void peek_does_not_affect_stats() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    (void)cache.peek(1);
    (void)cache.peek(2);
    CHK(cache.hitCount() == 0);
    CHK(cache.missCount() == 0);
}

// Executes all peek() test cases.
static void run_tests() {
    RUN(hit_returns_value);
    RUN(hit_does_not_update_recency);
    RUN(miss_returns_nullptr);
    RUN(peek_does_not_affect_stats);
}

REGISTER_TEST_SUITE();
