// LRUCache resize() test suite.
//
// Coverage:
// - Growing preserves all existing entries and raises capacity()
// - Shrinking to a size still >= the live entry count preserves everything
// - Shrinking below the live entry count evicts least-recently-used entries
//   first, keeping the most-recently-used ones
// - resize(0) throws std::invalid_argument

#include <support/framework.h>

using namespace CachePro;

// Verifies growing the cache preserves existing entries and raises capacity.
static void grow_preserves_entries() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.resize(4);
    CHK(cache.capacity() == 4);
    CHK(cache.size() == 2);
    CHK(cache.contains(1));
    CHK(cache.contains(2));

    cache.put(3, "c");
    cache.put(4, "d");
    CHK(cache.size() == 4);
    CHK(cache.contains(1)); // no eviction needed, room for all four
}

// Verifies shrinking to a size still covering all live entries keeps them all.
static void shrink_above_live_count_preserves_entries() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.resize(3);
    CHK(cache.capacity() == 3);
    CHK(cache.size() == 2);
    CHK(cache.contains(1));
    CHK(cache.contains(2));
}

// Verifies shrinking below the live entry count evicts the least-recently-used
// entries first, keeping the most-recently-used ones.
static void shrink_below_live_count_evicts_lru() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    cache.resize(2);
    CHK(cache.capacity() == 2);
    CHK(cache.size() == 2);
    CHK(!cache.contains(1)); // least-recently-used, evicted
    CHK(cache.contains(2));
    CHK(cache.contains(3));
}

// Verifies resize(0) is rejected.
static void resize_to_zero_throws() {
    LRUCache<int, std::string> cache(4);
    CHK_THROWS(cache.resize(0), std::invalid_argument);
}

// Executes all resize() test cases.
static void run_tests() {
    RUN(grow_preserves_entries);
    RUN(shrink_above_live_count_preserves_entries);
    RUN(shrink_below_live_count_evicts_lru);
    RUN(resize_to_zero_throws);
}

REGISTER_TEST_SUITE();
