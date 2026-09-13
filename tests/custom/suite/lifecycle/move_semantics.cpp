// LRUCache move semantics test suite.
//
// Coverage:
// - Move construction: the moved-to cache inherits capacity, size,
//   contents, and recency order
// - Move assignment: replaces the destination's existing contents entirely
// - Self-move-assignment is a safe no-op
// - The moved-from cache is left empty (size() == 0, capacity() == 0) and
//   remains safely destructible and reusable via move-assignment
// - Moving from an already-empty source, by construction or assignment,
//   takes the empty-list branch correctly

#include <support/framework.h>

using namespace CachePro;

// Verifies the moved-to cache takes over capacity, size, and contents.
static void move_construct_inherits_state() {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");
    source.put(2, "b");

    LRUCache<int, std::string> dest(std::move(source));
    CHK(dest.capacity() == 4);
    CHK(dest.size() == 2);
    CHK(*dest.peek(1) == "a");
    CHK(*dest.peek(2) == "b");
}

// Verifies the moved-to cache preserves recency order.
static void move_construct_preserves_recency() {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");
    source.put(2, "b");
    source.put(3, "c"); // MRU order: 3, 2, 1

    LRUCache<int, std::string> dest(std::move(source));
    CHK(*dest.mostRecentKey() == 3);
    CHK(*dest.leastRecentKey() == 1);
}

// Verifies the moved-from cache is left empty after move construction.
static void move_construct_leaves_source_empty() {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");

    LRUCache<int, std::string> dest(std::move(source));

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    CHK(source.size() == 0);
    CHK(source.capacity() == 0);
    CHK(source.empty());
}

// Verifies moving from an already-empty cache takes the empty-list branch.
static void move_construct_from_empty_source() {
    LRUCache<int, std::string> source(4);

    LRUCache<int, std::string> dest(std::move(source));
    CHK(dest.size() == 0);
    CHK(dest.capacity() == 4);
    CHK(dest.empty());
}

// Verifies move-assign replaces the destination's prior contents.
static void move_assign_replaces_contents() {
    LRUCache<int, std::string> dest(4);
    dest.put(100, "old");

    LRUCache<int, std::string> source(2);
    source.put(1, "a");
    source.put(2, "b");

    dest = std::move(source);
    CHK(dest.capacity() == 2);
    CHK(dest.size() == 2);
    CHK(!dest.contains(100)); // prior contents gone
    CHK(*dest.peek(1) == "a");
    CHK(*dest.peek(2) == "b");
}

// Verifies self-move-assignment is a safe no-op.
static void self_move_assign_is_safe() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache = std::move(cache);
    CHK(cache.size() == 2);
    CHK(*cache.peek(1) == "a");
    CHK(*cache.peek(2) == "b");
}

// Verifies the moved-from cache is left empty after move-assignment.
static void move_assign_leaves_source_empty() {
    LRUCache<int, std::string> dest(4);
    LRUCache<int, std::string> source(2);
    source.put(1, "a");

    dest = std::move(source);

    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    CHK(source.size() == 0);
    CHK(source.capacity() == 0);
    CHK(source.empty());
}

// Verifies move-assigning from an already-empty source takes the empty-list branch.
static void move_assign_from_empty_source() {
    LRUCache<int, std::string> dest(4);
    dest.put(1, "a");

    LRUCache<int, std::string> source(3);

    dest = std::move(source);
    CHK(dest.size() == 0);
    CHK(dest.capacity() == 3);
    CHK(dest.empty());
}

// Verifies a moved-from cache remains safely destructible and can be
// reused by move-assigning a fresh cache into it.
static void moved_from_cache_reusable_via_move_assign() {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");

    LRUCache<int, std::string> dest(std::move(source));

    LRUCache<int, std::string> replacement(2);
    replacement.put(9, "z");
    source = std::move(replacement);

    CHK(source.capacity() == 2);
    CHK(*source.peek(9) == "z");
}

// Executes all move construction and move assignment test cases.
static void run_tests() {
    RUN(move_construct_inherits_state);
    RUN(move_construct_preserves_recency);
    RUN(move_construct_leaves_source_empty);
    RUN(move_construct_from_empty_source);
    RUN(move_assign_replaces_contents);
    RUN(self_move_assign_is_safe);
    RUN(move_assign_leaves_source_empty);
    RUN(move_assign_from_empty_source);
    RUN(moved_from_cache_reusable_via_move_assign);
}

REGISTER_TEST_SUITE();
