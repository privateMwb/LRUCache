// LRUCache erase and clear test suite.
//
// Coverage:
// - erase() removes an existing entry
// - erase() returns false for a missing key
// - erase() frees a slot for reuse
// - erase() updates boundary keys
// - clear() removes all entries
// - clear() on an already-empty cache
// - clear() allows re-populating the cache

#include <common/framework.h>

using namespace CachePro;

// Verifies that erase() removes an existing entry and reports success.
static void erase_existing_key() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    bool erased = c.erase(1);

    CHK(erased == true);
    CHK(c.contains(1) == false);
    CHK(c.size() == 0);
}

// Verifies that erase() returns false when the key is absent.
static void erase_missing_key() {
    LRUCache<int, int> c(4);
    c.put(1, 10);

    bool erased = c.erase(99);

    CHK(erased == false);
    CHK(c.size() == 1);
}

// Verifies that erase() frees a slot that can be reused by a subsequent insertion.
static void erase_frees_slot_for_reuse() {
    LRUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);

    (void)c.erase(1);
    c.put(3, 30);

    CHK(c.size() == 2);
    CHK(c.contains(2) == true);
    CHK(c.contains(3) == true);
}

// Verifies that erase() updates the boundary keys correctly.
static void erase_updates_boundary_keys() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    (void)c.erase(3);

    CHK(*c.mostRecentKey() == 2);
}

// Verifies that clear() removes all entries and resets the cache.
static void clear_removes_all_entries() {
    LRUCache<int, int> c(4);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    c.clear();

    CHK(c.size() == 0);
    CHK(c.empty() == true);
    CHK(c.contains(1) == false);
    CHK(c.contains(2) == false);
    CHK(c.contains(3) == false);
}

// Verifies that clear() is safe to call on an already-empty cache.
static void clear_on_empty_cache() {
    LRUCache<int, int> c(4);
    c.clear();
    c.clear();

    CHK(c.size() == 0);
    CHK(c.empty() == true);
}

// Verifies that a cleared cache can be re-populated up to its capacity.
static void clear_allows_repopulation() {
    LRUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    c.put(3, 30);

    c.clear();

    c.put(4, 40);
    c.put(5, 50);
    c.put(6, 60);

    CHK(c.size() == 3);
    CHK(*c.get(4) == 40);
    CHK(*c.get(5) == 50);
    CHK(*c.get(6) == 60);
}

// Executes all erase and clear test cases.
static void run_tests() {
    RUN(erase_existing_key);
    RUN(erase_missing_key);
    RUN(erase_frees_slot_for_reuse);
    RUN(erase_updates_boundary_keys);
    RUN(clear_removes_all_entries);
    RUN(clear_on_empty_cache);
    RUN(clear_allows_repopulation);
}

REGISTER_TEST_SUITE();