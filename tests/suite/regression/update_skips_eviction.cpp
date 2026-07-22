// LRUCache update-skips-eviction regression test suite.
//
// Coverage:
// - put()/emplace() on a key that already exists must take the update
//   path even when the cache is at full capacity — it must NOT evict
//   another entry to make room, since no new slot is needed.
// - Every other entry present before the update must still be present
//   after it, with size() unchanged.

#include <support/framework.h>

using namespace CachePro;

// Verifies put() on an existing key at full capacity updates in place
// without evicting anything else.
static void put_update_at_capacity_does_not_evict() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(cache.size() == 2);

    cache.put(1, "updated");
    CHK(cache.size() == 2);
    CHK(cache.contains(1));
    CHK(cache.contains(2)); // must not have been evicted
    CHK(*cache.peek(1) == "updated");
}

// Verifies emplace() on an existing key at full capacity updates in place
// without evicting anything else.
static void emplace_update_at_capacity_does_not_evict() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(cache.size() == 2);

    cache.emplace(1, "updated");
    CHK(cache.size() == 2);
    CHK(cache.contains(2)); // must not have been evicted
    CHK(*cache.peek(1) == "updated");
}

// Verifies repeatedly updating the same key at full capacity never
// shrinks the live set below capacity via spurious eviction.
static void repeated_updates_at_capacity_preserve_all_entries() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "a");
    cache.put(2, "b");
    cache.put(3, "c");

    for (int i = 0; i < 20; ++i) {
        cache.put(2, "v" + std::to_string(i));
        CHK(cache.size() == 3);
        CHK(cache.contains(1));
        CHK(cache.contains(2));
        CHK(cache.contains(3));
    }
}

// Executes all update-skips-eviction test cases.
static void run_tests() {
    RUN(put_update_at_capacity_does_not_evict);
    RUN(emplace_update_at_capacity_does_not_evict);
    RUN(repeated_updates_at_capacity_preserve_all_entries);
}

REGISTER_TEST_SUITE();
