// LRUCache resize() allocation failure regression test suite.
//
// Coverage:
// - resize() allocates the new table (`new Slot[newTableCapacity]`) BEFORE
//   calling rebuildPool()/rebuildTable(). If that allocation throws, the
//   cache must be left completely untouched — old pool, old table, and all
//   existing entries still valid and usable.
// - Regression guard: an earlier ordering that freed the old pool before
//   allocating the new table would leave `table_` referencing freed
//   addresses on a failed allocation (use-after-free on the very next
//   operation).

#include <support/framework.h>

#include <limits>
#include <new>

using namespace CachePro;

// Verifies a resize() whose table allocation fails leaves the cache in its
// original, fully usable state.
static void resize_alloc_failure_leaves_cache_unchanged() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");

    CHK_THROWS(cache.resize(std::numeric_limits<std::size_t>::max() / 2), std::bad_alloc);

    // Nothing should have been committed: original capacity, size, and
    // contents must all still be intact.
    CHK(cache.capacity() == 3);
    CHK(cache.size() == 2);
    CHK(cache.contains(1));
    CHK(cache.contains(2));
    CHK(*cache.peek(1) == "a");
    CHK(*cache.peek(2) == "b");
}

// Verifies the cache remains fully functional (no use-after-free) after a
// failed resize() — the real regression this guards against.
static void cache_usable_after_resize_alloc_failure() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");

    try {
        cache.resize(std::numeric_limits<std::size_t>::max() / 2);
    } catch (const std::bad_alloc&) {
        // expected
    }

    // These would be use-after-free if the old pool/table had already
    // been freed before the failed allocation.
    cache.put(2, "b");
    CHK(cache.size() == 2);
    CHK(cache.contains(1));
    CHK(cache.contains(2));

    CHK(cache.erase(1));
    CHK(cache.size() == 1);
}

// Executes all resize() allocation failure test cases.
static void run_tests() {
    RUN(resize_alloc_failure_leaves_cache_unchanged);
    RUN(cache_usable_after_resize_alloc_failure);
}

REGISTER_TEST_SUITE();
