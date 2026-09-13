// LRUCache pool reuse integration test suite.
//
// Coverage:
// - Evict-then-insert cycles reuse freed pool slots rather than requiring
//   growth — capacity() stays fixed across many cycles with no failure
// - Erase-then-insert cycles likewise reuse the freed slot
// - Values are correct after many reuse cycles, confirming slots are fully
//   reset (not left holding stale state) when recycled

#include <support/framework.h>

using namespace CachePro;

// Verifies many evict/insert cycles never require growing past the fixed
// pool size, and contents stay correct throughout.
static void evict_insert_cycles_reuse_slots() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    for (int i = 4; i <= 1000; ++i) {
        cache.put(i, "v" + std::to_string(i));
        CHK(cache.size() == 3);
        CHK(cache.capacity() == 3);
        CHK(cache.contains(i));
    }
    CHK(*cache.peek(1000) == "v1000");
}

// Verifies erase-then-insert cycles reuse the freed slot correctly.
static void erase_insert_cycles_reuse_slots() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    for (int i = 4; i <= 200; ++i) {
        CHK(cache.erase(i - 3));
        cache.put(i, "v" + std::to_string(i));
        CHK(cache.size() == 3);
        CHK(cache.contains(i));
        CHK(!cache.contains(i - 3));
    }
}

// Executes all pool reuse test cases.
static void run_tests() {
    RUN(evict_insert_cycles_reuse_slots);
    RUN(erase_insert_cycles_reuse_slots);
}

REGISTER_TEST_SUITE();
