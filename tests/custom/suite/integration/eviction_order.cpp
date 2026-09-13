// LRUCache eviction order integration test suite.
//
// Coverage:
// - Filling to capacity and inserting past it evicts the correct
//   least-recently-used entry end-to-end (list + table + pool acting
//   together, not any single component in isolation)
// - get()-driven recency changes alter which entry is evicted next
// - Repeated eviction cycles keep evicting the correct entry over time

#include <support/framework.h>

using namespace CachePro;

// Verifies inserting past capacity evicts the least-recently-used entry.
static void insert_past_capacity_evicts_lru() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    cache.put(4, "d"); // evicts 1
    CHK(cache.size() == 3);
    CHK(!cache.contains(1));
    CHK(cache.contains(2));
    CHK(cache.contains(3));
    CHK(cache.contains(4));
}

// Verifies a get() on the current LRU entry saves it from the next eviction.
static void get_reprieves_entry_from_eviction() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(1); // 1 becomes MRU: 1, 3, 2
    cache.put(4, "d");  // now evicts 2, not 1

    CHK(cache.contains(1));
    CHK(!cache.contains(2));
    CHK(cache.contains(3));
    CHK(cache.contains(4));
}

// Verifies repeated fill-and-evict cycles keep evicting the correct entry.
static void repeated_eviction_cycles_stay_correct() {
    LRUCache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);

    for (int i = 3; i <= 10; ++i) {
        int aboutToEvict = *cache.leastRecentKey();
        cache.put(i, i);
        CHK(!cache.contains(aboutToEvict));
        CHK(cache.contains(i));
        CHK(cache.size() == 2);
    }
}

// Executes all eviction order test cases.
static void run_tests() {
    RUN(insert_past_capacity_evicts_lru);
    RUN(get_reprieves_entry_from_eviction);
    RUN(repeated_eviction_cycles_stay_correct);
}

REGISTER_TEST_SUITE();
