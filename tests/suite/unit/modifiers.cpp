// LRUCache modifier test suite.
//
// Coverage:
// - put(const K&, const V&)
// - put(const K&, V&&)
// - put(K&&, const V&)
// - put(K&&, V&&)
// - put on existing key updates value
// - put on existing key preserves size
// - put on existing key promotes entry to most-recently-used
// - emplace constructs a new entry
// - emplace on existing key updates value and promotes entry

#include <common/framework.h>

using namespace CachePro;

// Verifies that put(const K&, const V&) inserts a new entry.
static void put_lvalue_key_lvalue_value() {
    LRUCache<int, int> c(4);
    int key = 1, value = 10;
    c.put(key, value);

    CHK(c.size() == 1);
    CHK(*c.get(1) == 10);
}

// Verifies that put(const K&, V&&) moves the value into the cache.
static void put_lvalue_key_rvalue_value() {
    LRUCache<int, std::string> c(4);
    int key = 1;
    std::string value = "hello";
    c.put(key, std::move(value));

    CHK(c.size() == 1);
    CHK(*c.get(1) == "hello");
}

// Verifies that put(K&&, const V&) moves the key into the cache.
static void put_rvalue_key_lvalue_value() {
    LRUCache<std::string, int> c(4);
    std::string key = "a";
    int value = 42;
    c.put(std::move(key), value);

    CHK(c.size() == 1);
    CHK(*c.get("a") == 42);
}

// Verifies that put(K&&, V&&) moves both the key and value into the cache.
static void put_rvalue_key_rvalue_value() {
    LRUCache<std::string, std::string> c(4);
    c.put(std::string("k"), std::string("v"));

    CHK(c.size() == 1);
    CHK(*c.get("k") == "v");
}

// Verifies that putting an existing key updates its value.
static void put_existing_key_updates_value() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    c.put(1, 99);

    CHK(c.size() == 1);
    CHK(*c.get(1) == 99);
}

// Verifies that putting an existing key preserves the cache size.
static void put_existing_key_keeps_size() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    c.put(2, 20);
    c.put(1, 11);

    CHK(c.size() == 2);
}

// Verifies that putting an existing key promotes it to the most-recently-used position.
static void put_existing_key_moves_to_front() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    c.put(1, 15);

    CHK(*c.mostRecentKey() == 1);
}

// Verifies that emplace constructs a new entry when the key is absent.
static void emplace_constructs_new_entry() {
    LRUCache<int, std::string> c(4);
    std::string& v = c.emplace(1, "built");

    CHK(c.size() == 1);
    CHK(v == "built");
    CHK(*c.get(1) == "built");
}

// Verifies that emplace updates an existing entry and promotes it to the most-recently-used position.
static void emplace_existing_key_updates_and_promotes() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    c.emplace(1, 999);

    CHK(c.size() == 3);
    CHK(*c.get(1) == 999);
    CHK(*c.mostRecentKey() == 1);
}

// Executes all modifier test cases.
static void run_tests() {
    RUN(put_lvalue_key_lvalue_value);
    RUN(put_lvalue_key_rvalue_value);
    RUN(put_rvalue_key_lvalue_value);
    RUN(put_rvalue_key_rvalue_value);
    RUN(put_existing_key_updates_value);
    RUN(put_existing_key_keeps_size);
    RUN(put_existing_key_moves_to_front);
    RUN(emplace_constructs_new_entry);
    RUN(emplace_existing_key_updates_and_promotes);
}

REGISTER_TEST_SUITE();