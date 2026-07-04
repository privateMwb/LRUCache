// LRUCache lookup test suite.
//
// Coverage:
// - get() returns the correct value on hit
// - get() returns nullptr on miss
// - get() increments hit and miss counters
// - get() promotes an entry to the most-recently-used position
// - peek() returns the value without promotion
// - peek() does not affect hit or miss statistics
// - peek() returns nullptr on miss
// - contains() reports key presence accurately

#include <common/framework.h>

using namespace CachePro;

// Verifies that get() returns the correct value for an existing key.
static void get_returns_value_on_hit() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    int* v = c.get(1);

    CHK(v != nullptr);
    CHK(*v == 10);
}

// Verifies that get() returns nullptr when the key is absent.
static void get_returns_null_on_miss() {
    LRUCache<int, int> c(4);

    int* v = c.get(99);

    CHK(v == nullptr);
}

// Verifies that get() increments the hit counter on a successful lookup.
static void get_increments_hit_counter() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    (void)c.get(1);
    (void)c.get(1);

    CHK(c.hitCount() == 2);
    CHK(c.missCount() == 0);
}

// Verifies that get() increments the miss counter on a failed lookup.
static void get_increments_miss_counter() {
    LRUCache<int, int> c(4);

    (void)c.get(1);
    (void)c.get(2);

    CHK(c.missCount() == 2);
    CHK(c.hitCount() == 0);
}

// Verifies that get() promotes the accessed entry to the most-recently-used position.
static void get_promotes_entry() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    (void)c.get(1);

    CHK(*c.mostRecentKey() == 1);
}

// Verifies that peek() returns the value without promoting the entry.
static void peek_does_not_promote() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    const int* v = c.peek(1);

    CHK(v != nullptr);
    CHK(*v == 10);
    CHK(*c.mostRecentKey() == 3);
}

// Verifies that peek() does not affect hit or miss statistics.
static void peek_does_not_affect_stats() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    (void)c.peek(1);
    (void)c.peek(99);

    CHK(c.hitCount() == 0);
    CHK(c.missCount() == 0);
}

// Verifies that peek() returns nullptr when the key is absent.
static void peek_returns_null_on_miss() {
    LRUCache<int, int> c(4);

    const int* v = c.peek(1);

    CHK(v == nullptr);
}

// Verifies that contains() correctly reports whether a key is present.
static void contains_reports_presence() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    CHK(c.contains(1) == true);
    CHK(c.contains(99) == false);
}

// Executes all lookup test cases.
static void run_tests() {
    RUN(get_returns_value_on_hit);
    RUN(get_returns_null_on_miss);
    RUN(get_increments_hit_counter);
    RUN(get_increments_miss_counter);
    RUN(get_promotes_entry);
    RUN(peek_does_not_promote);
    RUN(peek_does_not_affect_stats);
    RUN(peek_returns_null_on_miss);
    RUN(contains_reports_presence);
}

REGISTER_TEST_SUITE();