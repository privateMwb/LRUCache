// LRUCache clear() test suite.
//
// Coverage:
// - All entries are removed: size() drops to 0, empty() becomes true
// - Previously stored keys are no longer found
// - capacity() is unchanged
// - The cache is fully reusable for further put()/get() calls afterward

#include <support/framework.h>

using namespace CachePro;

// Verifies clear() empties the cache.
static void clear_empties_cache() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.clear();
    CHK(cache.size() == 0);
    CHK(cache.empty());
    CHK(!cache.contains(1));
    CHK(!cache.contains(2));
}

// Verifies clear() leaves capacity unchanged.
static void clear_preserves_capacity() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    cache.clear();
    CHK(cache.capacity() == 4);
}

// Verifies the cache is reusable after clear().
static void cache_reusable_after_clear() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.clear();

    cache.put(2, "b");
    CHK(cache.size() == 1);
    CHK(*cache.peek(2) == "b");
}

// Executes all clear() test cases.
static void run_tests() {
    RUN(clear_empties_cache);
    RUN(clear_preserves_capacity);
    RUN(cache_reusable_after_clear);
}

REGISTER_TEST_SUITE();
