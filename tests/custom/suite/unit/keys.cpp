// LRUCache keys() test suite.
//
// Coverage:
// - Returns all keys ordered most-recently-used first
// - Reflects recency changes caused by get()/put()
// - An empty cache returns an empty vector

#include <support/framework.h>

using namespace CachePro;

// Verifies keys() returns entries in most-recently-used-first order.
static void keys_ordered_mru_first() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    std::vector<int> result = cache.keys();
    CHK(result.size() == 3);
    CHK(result[0] == 3);
    CHK(result[1] == 2);
    CHK(result[2] == 1);
}

// Verifies keys() reflects recency changes from a get().
static void keys_reflect_recency_updates() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    (void)cache.get(1); // 1 becomes MRU: 1, 3, 2

    std::vector<int> result = cache.keys();
    CHK(result[0] == 1);
    CHK(result[1] == 3);
    CHK(result[2] == 2);
}

// Verifies an empty cache returns an empty key list.
static void empty_cache_returns_empty_keys() {
    LRUCache<int, std::string> cache(4);
    CHK(cache.keys().empty());
}

// Executes all keys() test cases.
static void run_tests() {
    RUN(keys_ordered_mru_first);
    RUN(keys_reflect_recency_updates);
    RUN(empty_cache_returns_empty_keys);
}

REGISTER_TEST_SUITE();
