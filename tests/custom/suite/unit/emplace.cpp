// LRUCache emplace() test suite.
//
// Coverage:
// - Insert path constructs the value in place from the given arguments,
//   evicting the least-recently-used entry first if at capacity
// - Update path replaces the existing value with a freshly constructed one
//   and moves the entry to most-recently-used
// - Both paths return a reference to the (possibly newly inserted) value

#include <support/framework.h>

using namespace CachePro;

// Verifies emplace() on a new key constructs the value in place and returns it.
static void insert_path_constructs_value() {
    LRUCache<int, std::string> cache(4);
    std::string& value = cache.emplace(1, 3, 'x'); // std::string(3, 'x') == "xxx"
    CHK(value == "xxx");
    CHK(*cache.peek(1) == "xxx");
}

// Verifies emplace() evicts the least-recently-used entry when inserting at capacity.
static void insert_path_evicts_at_capacity() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(*cache.leastRecentKey() == 1);

    cache.emplace(3, "c");
    CHK(cache.size() == 2);
    CHK(!cache.contains(1)); // evicted
    CHK(cache.contains(2));
    CHK(cache.contains(3));
}

// Verifies emplace() on an existing key replaces the value and moves it to MRU.
static void update_path_replaces_and_moves_front() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    std::string& value = cache.emplace(1, 3, 'y'); // std::string(3, 'y') == "yyy"
    CHK(value == "yyy");
    CHK(*cache.peek(1) == "yyy");
    CHK(*cache.mostRecentKey() == 1);
    CHK(cache.size() == 2);
}

// Executes all emplace() test cases.
static void run_tests() {
    RUN(insert_path_constructs_value);
    RUN(insert_path_evicts_at_capacity);
    RUN(update_path_replaces_and_moves_front);
}

REGISTER_TEST_SUITE();
