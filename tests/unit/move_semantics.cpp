// LRUCache move semantics test suite.
//
// Coverage:
// - Moved-from cache is left in a valid empty state
// - Moved-from cache is safe to destroy
// - Chained moves transfer ownership correctly
// - Value pointer remains stable across move construction
// - Move assignment replaces the target's existing state

#include <common/framework.h>

using namespace CachePro;

// Verifies that a moved-from cache is left in a valid empty state.
static void moved_from_cache_is_safe_to_query() {
    LRUCache<int, int> source(4);
    source.put(1, 10);

    LRUCache<int, int> moved(std::move(source));

    CHK(source.size()      == 0);
    CHK(source.capacity()  == 0);
    CHK(source.empty()     == true);
    CHK(source.contains(1) == false);
}

// Verifies that a moved-from cache can be destroyed safely.
static void moved_from_cache_is_safe_to_destroy() {
    LRUCache<int, int> moved(4);
    {
        LRUCache<int, int> source(4);
        source.put(1, 10);
        moved = std::move(source);
        // source destructs here while moved-from.
    }

    CHK(moved.size() == 1);
    CHK(*moved.get(1) == 10);
}

// Verifies that chained moves preserve ownership of all entries.
static void chained_moves_transfer_ownership() {
    LRUCache<int, int> a(4);
    a.put(1, 10);
    a.put(2, 20);

    LRUCache<int, int> b(std::move(a));
    LRUCache<int, int> c(std::move(b));

    CHK(c.size() == 2);
    CHK(*c.get(1) == 10);
    CHK(*c.get(2) == 20);
    CHK(b.size() == 0);
    CHK(a.size() == 0);
}

// Verifies that value pointers remain stable across move construction.
static void value_pointer_stable_across_move() {
    LRUCache<int, int> source(4);
    source.put(1, 10);

    int* before = source.get(1);

    LRUCache<int, int> moved(std::move(source));
    int* after = moved.get(1);

    CHK(before == after);
}

// Verifies that move assignment replaces the target's existing state.
static void move_assignment_releases_prior_pool() {
    LRUCache<int, int> target(4);
    target.put(100, 1);
    target.put(200, 2);

    LRUCache<int, int> source(2);
    source.put(1, 10);

    target = std::move(source);

    CHK(target.size() == 1);
    CHK(target.contains(100) == false);
    CHK(target.contains(200) == false);
    CHK(*target.get(1) == 10);
}

// Executes all move semantics test cases.
static void run_tests() {
    RUN(moved_from_cache_is_safe_to_query);
    RUN(moved_from_cache_is_safe_to_destroy);
    RUN(chained_moves_transfer_ownership);
    RUN(value_pointer_stable_across_move);
    RUN(move_assignment_releases_prior_pool);
}

REGISTER_TEST_SUITE();