// LRUCache put/get roundtrip integration test suite.
//
// Coverage:
// - A basic full-cycle sanity check: putting a batch of entries and
//   reading them all back returns exactly the values that were stored
// - Overwriting a subset of keys and reading the whole batch back again
//   reflects the updates without disturbing the untouched entries

#include <support/framework.h>

using namespace CachePro;

// Verifies a batch of put() entries all read back correctly via get().
static void put_batch_then_get_all_roundtrips() {
    LRUCache<int, std::string> cache(10);
    for (int i = 0; i < 10; ++i) {
        cache.put(i, "value" + std::to_string(i));
    }

    for (int i = 0; i < 10; ++i) {
        std::string* value = cache.get(i);
        CHK(value != nullptr);
        CHK(*value == "value" + std::to_string(i));
    }
    CHK(cache.hitCount() == 10);
    CHK(cache.missCount() == 0);
}

// Verifies overwriting a subset of keys updates only those entries.
static void overwrite_subset_then_roundtrip() {
    LRUCache<int, std::string> cache(5);
    for (int i = 0; i < 5; ++i) {
        cache.put(i, "old" + std::to_string(i));
    }

    cache.put(1, "new1");
    cache.put(3, "new3");

    CHK(*cache.get(0) == "old0");
    CHK(*cache.get(1) == "new1");
    CHK(*cache.get(2) == "old2");
    CHK(*cache.get(3) == "new3");
    CHK(*cache.get(4) == "old4");
    CHK(cache.size() == 5);
}

// Executes all put/get roundtrip test cases.
static void run_tests() {
    RUN(put_batch_then_get_all_roundtrips);
    RUN(overwrite_subset_then_roundtrip);
}

REGISTER_TEST_SUITE();
