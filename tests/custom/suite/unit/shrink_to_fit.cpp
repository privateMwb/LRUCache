// LRUCache shrink_to_fit() test suite.
//
// Coverage:
// - Reclaims unused capacity: after shrink_to_fit(), capacity() == size()
// - No-op when the cache is empty (size() == 0)
// - No-op when the cache is already at full capacity (size() == capacity())
// - All existing entries, values, and recency order survive the shrink
// - The cache remains fully usable afterward (further put()s respect the
//   new, smaller capacity and evict correctly once full)

#include <support/framework.h>

using namespace CachePro;

// Verifies shrink_to_fit() reclaims unused capacity down to the live size.
static void shrink_to_fit_reclaims_unused_capacity() {
    LRUCache<int, std::string> cache(10);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");
    CHK(cache.capacity() == 10);

    cache.shrink_to_fit();
    CHK(cache.capacity() == 3);
    CHK(cache.size() == 3);
}

// Verifies shrink_to_fit() is a no-op on an empty cache.
static void shrink_to_fit_noop_when_empty() {
    LRUCache<int, std::string> cache(10);

    cache.shrink_to_fit();
    CHK(cache.capacity() == 10);
    CHK(cache.size() == 0);
    CHK(cache.empty());
}

// Verifies shrink_to_fit() is a no-op when already at full capacity.
static void shrink_to_fit_noop_when_full() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    cache.shrink_to_fit();
    CHK(cache.capacity() == 3);
    CHK(cache.size() == 3);
}

// Verifies all values and recency order survive the shrink.
static void shrink_to_fit_preserves_values_and_recency() {
    LRUCache<int, std::string> cache(8);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c"); // MRU order: 3, 2, 1

    cache.shrink_to_fit();
    CHK(cache.capacity() == 3);
    CHK(*cache.peek(1) == "a");
    CHK(*cache.peek(2) == "b");
    CHK(*cache.peek(3) == "c");
    CHK(*cache.mostRecentKey() == 3);
    CHK(*cache.leastRecentKey() == 1);
}

// Verifies the cache remains fully usable after shrinking, including
// correct eviction once the new, smaller capacity is reached.
static void cache_usable_after_shrink_to_fit() {
    LRUCache<int, std::string> cache(8);
    cache.put(1, "a");
    cache.put(2, "b");

    cache.shrink_to_fit();
    CHK(cache.capacity() == 2);

    cache.put(3, "c"); // now at capacity, no eviction yet
    CHK(cache.size() == 2);
    CHK(!cache.contains(1)); // 1 was LRU, evicted to make room
    CHK(cache.contains(2));
    CHK(cache.contains(3));
}

// Executes all shrink_to_fit() test cases.
static void run_tests() {
    RUN(shrink_to_fit_reclaims_unused_capacity);
    RUN(shrink_to_fit_noop_when_empty);
    RUN(shrink_to_fit_noop_when_full);
    RUN(shrink_to_fit_preserves_values_and_recency);
    RUN(cache_usable_after_shrink_to_fit);
}

REGISTER_TEST_SUITE();
