// LRUCache edge case test suite.
//
// Coverage:
// - Capacity of 1 behaves correctly
// - Move-only value types are supported
// - Non-default-constructible value types are supported
// - std::string keys are supported
// - Repeated put() on the same key does not increase cache size

#include <common/framework.h>

#include <memory>

using namespace CachePro;

// A value type with no default constructor.
struct NoDefault {
    int x;
    explicit NoDefault(int v) : x(v) {}
    NoDefault() = delete;
};

// Verifies that a capacity of 1 evicts the existing entry on each new insertion.
static void capacity_one_evicts_every_insert() {
    LRUCache<int, int> c(1);
    c.put(1, 10);
    c.put(2, 20);

    CHK(c.size() == 1);
    CHK(c.contains(1) == false);
    CHK(c.contains(2) == true);
}

// Verifies that a capacity of 1 allows repeated updates to the same key.
static void capacity_one_allows_same_key_updates() {
    LRUCache<int, int> c(1);
    c.put(1, 10);
    c.put(1, 20);
    c.put(1, 30);

    CHK(c.size() == 1);
    CHK(*c.get(1) == 30);
}

// Verifies that move-only value types are supported.
static void move_only_value_type() {
    LRUCache<int, std::unique_ptr<int>> c(2);
    c.put(1, std::make_unique<int>(42));

    auto* v = c.get(1);

    CHK(v != nullptr);
    CHK(**v == 42);
}

// Verifies that move-only values remain valid after evicting another entry.
static void move_only_value_survives_eviction() {
    LRUCache<int, std::unique_ptr<int>> c(1);
    c.put(1, std::make_unique<int>(1));
    c.put(2, std::make_unique<int>(2));

    CHK(c.contains(1) == false);
    CHK(**c.get(2) == 2);
}

// Verifies that non-default-constructible value types are supported.
static void non_default_constructible_value() {
    LRUCache<int, NoDefault> c(2);
    c.put(1, NoDefault(7));
    c.put(2, NoDefault(8));

    CHK(c.get(1)->x == 7);
    CHK(c.get(2)->x == 8);

    c.put(3, NoDefault(9));

    CHK(c.contains(1) == false);
    CHK(c.get(3)->x == 9);
}

// Verifies that std::string keys support lookup and eviction correctly.
static void string_key_lookup_and_eviction() {
    LRUCache<std::string, int> c(2);
    c.put("alpha", 1);
    c.put("beta", 2);
    c.put("gamma", 3);

    CHK(c.contains("alpha") == false);
    CHK(*c.get("beta") == 2);
    CHK(*c.get("gamma") == 3);
}

// Verifies that repeated put() operations on the same key do not increase cache size.
static void repeated_same_key_put_never_grows() {
    LRUCache<int, int> c(4);
    for (int i = 0; i < 50; ++i) {
        c.put(1, i);
    }

    CHK(c.size() == 1);
    CHK(*c.get(1) == 49);
}

// Executes all edge case test cases.
static void run_tests() {
    RUN(capacity_one_evicts_every_insert);
    RUN(capacity_one_allows_same_key_updates);
    RUN(move_only_value_type);
    RUN(move_only_value_survives_eviction);
    RUN(non_default_constructible_value);
    RUN(string_key_lookup_and_eviction);
    RUN(repeated_same_key_put_never_grows);
}

REGISTER_TEST_SUITE();