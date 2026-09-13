// LRUCache moveToFront() no-op regression test suite.
//
// Coverage:
// - moveToFront() early-returns when the node is already the
//   most-recently-used entry (head_.next == node), skipping the
//   unlink/relink. This suite pins down that the early return leaves the
//   list links completely intact — no self-linking corruption, no lost
//   neighbors — and that later, real moves still work correctly afterward.

#include <support/framework.h>

#include <vector>

using namespace CachePro;

// Verifies repeatedly touching the already-MRU entry doesn't disturb list order.
static void repeated_touch_of_front_entry_is_noop() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(3); // already MRU — exercises the early-return branch
    (void)cache.get(3);
    (void)cache.get(3);

    CHK(*cache.mostRecentKey() == 3);
    CHK(*cache.leastRecentKey() == 1);

    std::vector<int> keys = cache.keys();
    CHK(keys.size() == 3);
    CHK(keys[0] == 3);
    CHK(keys[1] == 2);
    CHK(keys[2] == 1);
}

// Verifies a real move still works correctly right after the no-op branch fires.
static void noop_touch_does_not_break_subsequent_real_move() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(3); // no-op branch
    (void)cache.get(1); // real move: 1 becomes MRU

    std::vector<int> keys = cache.keys();
    CHK(keys[0] == 1);
    CHK(keys[1] == 3);
    CHK(keys[2] == 2);
}

// Verifies the no-op branch on a single-entry cache leaves it fully intact.
static void noop_touch_on_single_entry_cache() {
    LRUCache<int, std::string> cache(1);
    cache.put(1, "a");

    (void)cache.get(1);
    (void)cache.get(1);

    CHK(*cache.mostRecentKey() == 1);
    CHK(*cache.leastRecentKey() == 1);
    CHK(cache.size() == 1);
}

// Executes all moveToFront() no-op test cases.
static void run_tests() {
    RUN(repeated_touch_of_front_entry_is_noop);
    RUN(noop_touch_does_not_break_subsequent_real_move);
    RUN(noop_touch_on_single_entry_cache);
}

REGISTER_TEST_SUITE();
