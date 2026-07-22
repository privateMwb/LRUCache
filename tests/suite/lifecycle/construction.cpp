// LRUCache construction test suite.
//
// Coverage:
// - Valid capacity constructs an empty cache with correct capacity()/size()
// - Pool and table are sized for the full requested capacity at construction
//   (all `capacity` distinct inserts succeed with no premature eviction)
// - Zero capacity throws std::invalid_argument
// - Capacity of 1 is a valid edge case and behaves like any other capacity

#include <support/framework.h>

using namespace CachePro;

// Verifies a freshly constructed cache reports the requested capacity and starts empty.
static void valid_capacity_constructs_empty() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.capacity() == 4);
    CHK(cache.size() == 0);
    CHK(cache.empty());
}

// Verifies the pool and table are allocated to hold the full requested
// capacity up front — inserting exactly `capacity` distinct keys never
// triggers an early eviction.
static void pool_and_table_sized_for_full_capacity() {
    LRUCache<int, std::string> cache(5);
    for (int i = 0; i < 5; ++i) {
        cache.put(i, "v");
    }
    CHK(cache.size() == 5);
    for (int i = 0; i < 5; ++i) {
        CHK(cache.contains(i));
    }
}

// Verifies capacity of 0 is rejected.
static void zero_capacity_throws() {
    CHK_THROWS((LRUCache<int, std::string>(0)), std::invalid_argument);
}

// Verifies capacity of 1 is a valid edge case: single entry fits, second put evicts it.
static void capacity_one_edge_case() {
    LRUCache<int, std::string> cache(1);
    cache.put(1, "a");
    CHK(cache.size() == 1);
    CHK(cache.contains(1));

    cache.put(2, "b");
    CHK(cache.size() == 1);
    CHK(!cache.contains(1));
    CHK(cache.contains(2));
}

// Executes all construction test cases.
static void run_tests() {
    RUN(valid_capacity_constructs_empty);
    RUN(pool_and_table_sized_for_full_capacity);
    RUN(zero_capacity_throws);
    RUN(capacity_one_edge_case);
}

REGISTER_TEST_SUITE();
