// LRUCache statistics test suite.
//
// Coverage:
// - hitCount() and missCount() track get() outcomes
// - hitRate() computes the correct percentage
// - hitRate() returns 0.0 when no accesses have been recorded
// - resetStats() resets both counters
// - put(), peek(), and contains() do not affect statistics

#include <common/framework.h>

using namespace CachePro;

// Verifies that hitCount() and missCount() track get() outcomes correctly.
static void stats_track_hits_and_misses() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    (void)c.get(1);
    (void)c.get(1);
    (void)c.get(99);

    CHK(c.hitCount()  == 2);
    CHK(c.missCount() == 1);
}

// Verifies that hitRate() computes the correct percentage.
static void hit_rate_computes_percentage() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    (void)c.get(1);
    (void)c.get(1);
    (void)c.get(1);
    (void)c.get(99);

    CHK(c.hitRate() == 75.0);
}

// Verifies that hitRate() returns 0.0 when no accesses have been recorded.
static void hit_rate_zero_when_no_accesses() {
    LRUCache<int, int> c(4);

    CHK(c.hitRate() == 0.0);
}

// Verifies that resetStats() resets both counters.
static void reset_stats_zeroes_counters() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    (void)c.get(1);
    (void)c.get(99);

    c.resetStats();

    CHK(c.hitCount()  == 0);
    CHK(c.missCount() == 0);
}

// Verifies that put(), peek(), and contains() do not affect cache statistics.
static void non_get_operations_do_not_affect_stats() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    (void)c.peek(1);
    (void)c.contains(1);
    (void)c.contains(99);

    CHK(c.hitCount()  == 0);
    CHK(c.missCount() == 0);
}

// Executes all statistics test cases.
static void run_tests() {
    RUN(stats_track_hits_and_misses);
    RUN(hit_rate_computes_percentage);
    RUN(hit_rate_zero_when_no_accesses);
    RUN(reset_stats_zeroes_counters);
    RUN(non_get_operations_do_not_affect_stats);
}

REGISTER_TEST_SUITE();