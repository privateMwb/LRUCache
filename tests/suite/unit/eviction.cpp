// LRUCache eviction test suite.
//
// Coverage:
// - Eviction on capacity overflow
// - Correct victim is the least-recently-used entry
// - get() promotion protects an entry from eviction
// - Cache size never exceeds capacity
// - Stable behavior across repeated evict/insert cycles
// - leastRecentKey reports the next eviction candidate

#include <common/framework.h>

using namespace CachePro;

// Verifies that inserting past capacity evicts the least-recently-used entry.
static void eviction_removes_lru_entry() {
    LRUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    CHK(c.size() == 2);
    CHK(c.contains(1) == false);
    CHK(c.contains(2) == true);
    CHK(c.contains(3) == true);
}

// Verifies that get() promotes an entry and protects it from eviction.
static void get_promotion_protects_entry() {
    LRUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);

    (void)c.get(1);          // 1 becomes most-recently-used
    c.put(3, 30);      // Should evict 2, not 1.

    CHK(c.contains(1) == true);
    CHK(c.contains(2) == false);
    CHK(c.contains(3) == true);
}

// Verifies that cache size never exceeds its capacity.
static void eviction_never_exceeds_capacity() {
    LRUCache<int, int> c(3);
    for (int i = 0; i < 100; ++i) {
        c.put(i, i * 10);
        CHK(c.size() <= c.capacity());
    }

    CHK(c.size() == 3);
}

// Verifies that repeated eviction and insertion cycles preserve cache integrity.
static void repeated_eviction_cycles_stable() {
    LRUCache<int, int> c(4);

    for (int i = 0; i < 10000; ++i) {
        c.put(i, i);
    }
    
    CHK(c.size() == 4);

    for (int i = 9996; i < 10000; ++i) {
        CHK(c.contains(i) == true);
    }
}

// Verifies that leastRecentKey identifies the next entry to be evicted.
static void least_recent_key_matches_eviction_order() {
    LRUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);

    CHK(*c.leastRecentKey() == 1);

    c.put(3, 30);

    CHK(c.contains(1) == false);
}

// Executes all eviction test cases.
static void run_tests() {
    RUN(eviction_removes_lru_entry);
    RUN(get_promotion_protects_entry);
    RUN(eviction_never_exceeds_capacity);
    RUN(repeated_eviction_cycles_stable);
    RUN(least_recent_key_matches_eviction_order);
}

REGISTER_TEST_SUITE();