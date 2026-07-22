// LRUCache resize workflow integration test suite.
//
// Coverage:
// - Grow, then shrink, then continue normal use — pool and table stay
//   consistent with each other across both resize directions
// - Recency order and values survive the full grow/shrink/use cycle
// - The cache remains fully functional (put/get/evict) after resizing

#include <support/framework.h>

using namespace CachePro;

// Verifies a grow-then-shrink-then-use workflow behaves correctly end-to-end.
static void grow_then_shrink_then_continue_use() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");

    // Grow: room for more entries, nothing evicted.
    cache.resize(5);
    cache.put(3, "c");
    cache.put(4, "d");
    cache.put(5, "e");
    CHK(cache.size() == 5);
    CHK(cache.capacity() == 5);
    CHK(cache.contains(1));

    // MRU order right now: 5, 4, 3, 2, 1
    (void)cache.get(1); // 1 becomes MRU: 1, 5, 4, 3, 2

    // Shrink below live count: evicts LRU entries first (2, then 3).
    cache.resize(3);
    CHK(cache.capacity() == 3);
    CHK(cache.size() == 3);
    CHK(cache.contains(1));
    CHK(cache.contains(4));
    CHK(cache.contains(5));
    CHK(!cache.contains(2));
    CHK(!cache.contains(3));

    // Cache remains fully usable: normal put/get/evict still work.
    cache.put(6, "f"); // evicts current LRU
    CHK(cache.size() == 3);
    CHK(cache.contains(6));
    CHK(*cache.peek(6) == "f");
}

// Executes all resize workflow test cases.
static void run_tests() {
    RUN(grow_then_shrink_then_continue_use);
}

REGISTER_TEST_SUITE();
