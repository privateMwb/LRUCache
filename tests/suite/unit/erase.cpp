// LRUCache erase() test suite.
//
// Coverage:
// - Existing key: returns true, removes the entry, decrements size()
// - Missing key: returns false, leaves the cache unchanged
// - Post-erase state: capacity() is unchanged and the freed slot is
//   reusable by a subsequent put()

#include <support/framework.h>

using namespace CachePro;

// Verifies erasing an existing key succeeds and removes it.
static void existing_key_erased() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");
    cache.put(2, "b");

    CHK(cache.erase(1));
    CHK(!cache.contains(1));
    CHK(cache.size() == 1);
}

// Verifies erasing a missing key fails and changes nothing.
static void missing_key_not_erased() {
    LRUCache<int, std::string> cache(4);
    cache.put(1, "a");

    CHK(!cache.erase(2));
    CHK(cache.size() == 1);
    CHK(cache.contains(1));
}

// Verifies capacity is unaffected by erase(), and the freed slot can be reused.
static void post_erase_state_and_slot_reuse() {
    LRUCache<int, std::string> cache(2);
    cache.put(1, "a");
    cache.put(2, "b");
    CHK(cache.erase(1));
    CHK(cache.capacity() == 2);

    cache.put(3, "c"); // should reuse the freed slot, not evict
    CHK(cache.size() == 2);
    CHK(cache.contains(2));
    CHK(cache.contains(3));
}

// Executes all erase() test cases.
static void run_tests() {
    RUN(existing_key_erased);
    RUN(missing_key_not_erased);
    RUN(post_erase_state_and_slot_reuse);
}

REGISTER_TEST_SUITE();
