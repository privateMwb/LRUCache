// LRUCache full-load guard regression test suite.
//
// Coverage:
// - tableSizeFor() always rounds up to a power of two >= 2*capacity,
//   guaranteeing at least one permanently empty slot no matter how full
//   the cache gets. findSlot()/insertSlot()'s linear probe loops rely on
//   hitting that empty slot to terminate — this suite pins down the
//   tightest case (capacity 1, table size 2) where that guarantee is most
//   load-bearing, since it's the smallest table size the class ever builds.

#include <support/framework.h>

using namespace CachePro;

// Verifies a lookup for a missing key terminates correctly at the tightest
// possible load (capacity 1, so the table is always exactly half full).
static void find_slot_terminates_at_capacity_one() {
    LRUCache<int, std::string> cache(1);
    cache.put(1, "a"); // 1 of 2 table slots occupied

    CHK(!cache.contains(2));
    CHK(cache.get(2) == nullptr);
    CHK(cache.peek(2) == nullptr);
}

// Verifies many put() cycles at capacity 1 terminate and stay correct —
// each insert always finds the guaranteed empty slot to land in.
static void put_cycles_terminate_at_capacity_one() {
    LRUCache<int, std::string> cache(1);
    for (int i = 0; i < 500; ++i) {
        cache.put(i, "v");
        CHK(cache.size() == 1);
        CHK(cache.contains(i));
    }
}

// Verifies erase-then-reinsert cycles at capacity 1 also terminate and
// stay correct, alternating which of the two table slots ends up occupied.
static void erase_and_reinsert_cycles_terminate_at_capacity_one() {
    LRUCache<int, std::string> cache(1);
    int currentKey = 1;
    cache.put(currentKey, "v");

    for (int i = 2; i < 200; ++i) {
        CHK(cache.erase(currentKey));
        cache.put(i, "v");
        CHK(cache.contains(i));
        CHK(cache.size() == 1);
        currentKey = i;
    }
}

// Executes all full-load guard test cases.
static void run_tests() {
    RUN(find_slot_terminates_at_capacity_one);
    RUN(put_cycles_terminate_at_capacity_one);
    RUN(erase_and_reinsert_cycles_terminate_at_capacity_one);
}

REGISTER_TEST_SUITE();