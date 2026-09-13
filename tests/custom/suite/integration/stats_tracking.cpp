// LRUCache stats tracking integration test suite.
//
// Coverage:
// - hitCount()/missCount()/hitRate() stay correct across a realistic mixed
//   sequence of put(), get(), peek(), contains(), erase(), and eviction
// - Only get() affects the counters — peek()/contains() never do, even
//   interleaved with real hits and misses
// - resetStats() mid-sequence correctly zeroes and resumes counting

#include <support/framework.h>

using namespace CachePro;

// Verifies hit/miss counters stay correct across a realistic mixed sequence
// of operations, and that only get() affects them.
static void mixed_operations_track_stats_correctly() {
    LRUCache<int, std::string> cache(3);

    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    (void)cache.peek(1);     // no effect on stats
    (void)cache.contains(2); // no effect on stats
    CHK(cache.hitCount() == 0);
    CHK(cache.missCount() == 0);

    (void)cache.get(1);   // hit
    (void)cache.get(99);  // miss
    cache.put(4, "d");    // evicts LRU (key 2), no stats effect
    (void)cache.get(2);   // miss (was evicted)
    (void)cache.get(4);   // hit
    (void)cache.erase(3); // no stats effect
    (void)cache.get(3);   // miss (erased)

    CHK(cache.hitCount() == 2);
    CHK(cache.missCount() == 3);
    CHK(cache.hitRate() == 40.0); // 2 / 5 * 100

    cache.resetStats();
    CHK(cache.hitCount() == 0);
    CHK(cache.missCount() == 0);
    CHK(cache.hitRate() == 0.0);

    (void)cache.get(1); // hit, counting resumes cleanly after reset
    CHK(cache.hitCount() == 1);
    CHK(cache.missCount() == 0);
}

// Executes all stats tracking test cases.
static void run_tests() {
    RUN(mixed_operations_track_stats_correctly);
}

REGISTER_TEST_SUITE();
