// LRUCache constructor and move semantics test suite.
//
// Coverage:
// - Valid capacity construction
// - Zero capacity throws
// - Move construction (empty cache)
// - Move construction (populated cache)
// - Move assignment (empty target)
// - Move assignment (overwrites populated target)
// - Self move assignment

#include <common/framework.h>

using namespace CachePro;

// Verifies that a valid capacity constructs an empty cache.
static void valid_capacity_construction() {
    LRUCache<int, int> c(4);
    CHK(c.capacity() == 4);
    CHK(c.size()     == 0);
    CHK(c.empty()    == true);
}

// Verifies that zero capacity throws std::invalid_argument.
static void zero_capacity_throws() {
    bool threw = false;
    try {
        LRUCache<int, int> c(0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHK(threw == true);
}

// Verifies that move construction transfers ownership from an empty cache.
static void move_construction_empty() {
    LRUCache<int, int> source(4);
    LRUCache<int, int> moved(std::move(source));

    CHK(moved.capacity() == 4);
    CHK(moved.size()     == 0);
    CHK(source.capacity() == 0);
    CHK(source.size()     == 0);
}

// Verifies that move construction transfers all entries from a populated cache.
static void move_construction_populated() {
    LRUCache<int, int> source(3);
    source.put(1, 10);
    source.put(2, 20);
    source.put(3, 30);

    LRUCache<int, int> moved(std::move(source));

    CHK(moved.size()      == 3);
    CHK(*moved.get(1)     == 10);
    CHK(*moved.get(2)     == 20);
    CHK(*moved.get(3)     == 30);
    CHK(source.capacity() == 0);
    CHK(source.size()     == 0);
}

// Verifies that move assignment transfers ownership into an empty target.
static void move_assignment_empty_target() {
    LRUCache<int, int> source(4);
    source.put(1, 100);

    LRUCache<int, int> target(2);
    target = std::move(source);

    CHK(target.capacity() == 4);
    CHK(target.size()     == 1);
    CHK(*target.get(1)    == 100);
    CHK(source.capacity() == 0);
    CHK(source.size()     == 0);
}

// Verifies that move assignment overwrites the target's existing state.
static void move_assignment_overwrites_target() {
    LRUCache<int, int> source(2);
    source.put(1, 111);

    LRUCache<int, int> target(3);
    target.put(9, 999);
    target.put(8, 888);

    target = std::move(source);

    CHK(target.size()        == 1);
    CHK(*target.get(1)       == 111);
    CHK(target.contains(9)   == false);
    CHK(target.contains(8)   == false);
}

// Verifies that self move assignment leaves the cache unchanged.
static void self_move_assignment() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);

    c = std::move(c);

    CHK(c.size()  == 2);
    CHK(*c.get(1) == 10);
    CHK(*c.get(2) == 20);
}

// Executes all constructor and move semantics test cases.
static void run_tests() {
    RUN(valid_capacity_construction);
    RUN(zero_capacity_throws);
    RUN(move_construction_empty);
    RUN(move_construction_populated);
    RUN(move_assignment_empty_target);
    RUN(move_assignment_overwrites_target);
    RUN(self_move_assignment);
}

REGISTER_TEST_SUITE();