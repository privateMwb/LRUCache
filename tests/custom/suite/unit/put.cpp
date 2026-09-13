// LRUCache put() test suite.
//
// Coverage:
// - All four put() overloads: copy/copy, copy key + move value, move key +
//   copy value, move/move
// - Insert path adds a new entry as most-recently-used
// - Update path replaces the value in place, leaves size unchanged, and
//   moves the existing entry to most-recently-used

#include <support/framework.h>

using namespace CachePro;

// Verifies put(const K&, const V&) inserts a retrievable entry.
static void copy_key_copy_value() {
    LRUCache<int, std::string> cache(4);
    int key = 1;
    std::string value = "one";
    cache.put(key, value);
    CHK(*cache.peek(1) == "one");
}

// Verifies put(const K&, V&&) inserts a retrievable entry.
static void copy_key_move_value() {
    LRUCache<int, std::string> cache(4);
    int key = 1;
    cache.put(key, std::string("one"));
    CHK(*cache.peek(1) == "one");
}

// Verifies put(K&&, const V&) inserts a retrievable entry.
static void move_key_copy_value() {
    LRUCache<int, std::string> cache(4);
    std::string value = "one";
    cache.put(1, value);
    CHK(*cache.peek(1) == "one");
}

// Verifies put(K&&, V&&) inserts a retrievable entry.
static void move_key_move_value() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, std::string("one"));
    CHK(*cache.peek(1) == "one");
}

// Verifies inserting a new key becomes the most-recently-used entry.
static void insert_path_becomes_mru() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(*cache.mostRecentKey() == 2);
}

// Verifies updating an existing key replaces the value, leaves size
// unchanged, and moves the entry to most-recently-used.
static void update_path_replaces_and_moves_front() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(cache.size() == 2);

    cache.put(1, "updated");
    CHK(cache.size() == 2);
    CHK(*cache.peek(1) == "updated");
    CHK(*cache.mostRecentKey() == 1);
}

// Executes all put() test cases.
static void run_tests() {
    RUN(copy_key_copy_value);
    RUN(copy_key_move_value);
    RUN(move_key_copy_value);
    RUN(move_key_move_value);
    RUN(insert_path_becomes_mru);
    RUN(update_path_replaces_and_moves_front);
}

REGISTER_TEST_SUITE();
