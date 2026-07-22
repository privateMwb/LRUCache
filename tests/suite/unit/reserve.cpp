// LRUCache reserve() test suite.
//
// Coverage:
// - reserve() is a documented no-op: capacity(), size(), and existing
//   entries are unaffected regardless of the argument passed in

#include <support/framework.h>

using namespace CachePro;

// Verifies reserve() does not change capacity or size.
static void reserve_does_not_change_capacity_or_size() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    cache.reserve(100);
    CHK(cache.capacity() == 4);
    CHK(cache.size() == 1);
}

// Verifies reserve() leaves existing entries untouched.
static void reserve_does_not_disturb_entries() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.reserve(0);
    CHK(cache.contains(1));
    CHK(cache.contains(2));
    CHK(*cache.peek(1) == "a");
}

// Executes all reserve() test cases.
static void run_tests() {
    RUN(reserve_does_not_change_capacity_or_size);
    RUN(reserve_does_not_disturb_entries);
}

REGISTER_TEST_SUITE();
