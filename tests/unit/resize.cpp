// LRUCache resize test suite.
//
// Coverage:
// - Growing capacity preserves existing entries
// - Growing capacity allows additional insertions
// - Shrinking capacity evicts least-recently-used entries
// - Shrinking to the current size evicts nothing
// - Resizing to zero throws
// - Shrinking preserves the most-recently-used entry

#include <common/framework.h>

using namespace CachePro;

// Verifies that growing capacity preserves all existing entries.
static void resize_grow_preserves_entries() {
    LRUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);

    c.resize(4);

    CHK(c.capacity() == 4);
    CHK(c.size() == 2);
    CHK(*c.get(1) == 10);
    CHK(*c.get(2) == 20);
}

// Verifies that growing capacity allows additional insertions without eviction.
static void resize_grow_allows_more_insertions() {
    LRUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);

    c.resize(4);
    c.put(3, 30);
    c.put(4, 40);

    CHK(c.size() == 4);
    CHK(c.contains(1) == true);
    CHK(c.contains(2) == true);
}

// Verifies that shrinking capacity evicts the least-recently-used entries.
static void resize_shrink_evicts_lru() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);
    c.put(4, 40);

    c.resize(2);

    CHK(c.capacity() == 2);
    CHK(c.size() == 2);
    CHK(c.contains(1) == false);
    CHK(c.contains(2) == false);
    CHK(c.contains(3) == true);
    CHK(c.contains(4) == true);
}

// Verifies that shrinking to the current size preserves all entries.
static void resize_shrink_to_current_size() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    c.put(2, 20);

    c.resize(2);

    CHK(c.capacity() == 2);
    CHK(c.size() == 2);
    CHK(c.contains(1) == true);
    CHK(c.contains(2) == true);
}

// Verifies that resizing to zero throws std::invalid_argument.
static void resize_to_zero_throws() {
    LRUCache<int, int> c(4);
    bool threw = false;

    try {
        c.resize(0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    CHK(threw == true);
}

// Verifies that shrinking preserves the most-recently-used entry.
static void resize_shrink_preserves_mru_order() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);
    (void)c.get(1);   // 1 is now most-recently-used.

    c.resize(1);

    CHK(c.size() == 1);
    CHK(c.contains(1) == true);
}

// Executes all resize test cases.
static void run_tests() {
    RUN(resize_grow_preserves_entries);
    RUN(resize_grow_allows_more_insertions);
    RUN(resize_shrink_evicts_lru);
    RUN(resize_shrink_to_current_size);
    RUN(resize_to_zero_throws);
    RUN(resize_shrink_preserves_mru_order);
}

REGISTER_TEST_SUITE();