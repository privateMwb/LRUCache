// LRUCache erase() wraparound regression test suite.
//
// Coverage:
// - eraseSlot()'s backward-shift deletion must stay correct when the probe
//   cluster it's compacting wraps past the end of the table back to index 0
//   — the distance calculations (`distToHole`, `distToProbe`) use modular
//   arithmetic specifically to handle this, and this suite pins it down.
// - Uses a colliding Hash functor to force every key into the same home
//   slot near the end of the table, guaranteeing the resulting cluster
//   spans the wraparound boundary.

#include <support/framework.h>

using namespace CachePro;

namespace {

// Forces every key to the same home slot, so linear probing is guaranteed
// to build a cluster that wraps from the end of the table back to index 0.
struct CollidingHash {
    std::size_t operator()(int) const noexcept {
        return 7;
    }
};

using WrapCache = LRUCache<int, std::string, CollidingHash>;

} // namespace

// Verifies erasing an entry in the wrapped portion of a cluster leaves the
// rest of the cluster fully reachable.
static void erase_middle_of_wrapped_cluster_stays_reachable() {
    // capacity 3 -> table size 8 (tableSizeFor(3) = 8), mask 7.
    // All three keys hash to 7, so they land at slots 7, 0, 1 in order —
    // a cluster that wraps across the table boundary.
    WrapCache cache(3);
    cache.put(1, "a"); // slot 7
    cache.put(2, "b"); // slot 0 (wrapped)
    cache.put(3, "c"); // slot 1 (wrapped)

    CHK(cache.erase(2)); // erase the middle, wrapped entry

    CHK(cache.contains(1));
    CHK(!cache.contains(2));
    CHK(cache.contains(3));
    CHK(*cache.peek(1) == "a");
    CHK(*cache.peek(3) == "c");
}

// Verifies erasing the entry at the cluster's true home slot (just before
// the wrap) still leaves the wrapped entries reachable.
static void erase_home_slot_before_wrap_keeps_wrapped_entries() {
    WrapCache cache(3);
    cache.put(1, "a"); // slot 7 (home slot, pre-wrap)
    cache.put(2, "b"); // slot 0 (wrapped)
    cache.put(3, "c"); // slot 1 (wrapped)

    CHK(cache.erase(1));

    CHK(!cache.contains(1));
    CHK(cache.contains(2));
    CHK(cache.contains(3));
    CHK(*cache.peek(2) == "b");
    CHK(*cache.peek(3) == "c");
}

// Verifies erasing the last entry in a wrapped cluster leaves the earlier
// (pre-wrap) entries reachable.
static void erase_end_of_wrapped_cluster_keeps_earlier_entries() {
    WrapCache cache(3);
    cache.put(1, "a"); // slot 7
    cache.put(2, "b"); // slot 0
    cache.put(3, "c"); // slot 1

    CHK(cache.erase(3));

    CHK(cache.contains(1));
    CHK(cache.contains(2));
    CHK(!cache.contains(3));
}

// Executes all erase() wraparound test cases.
static void run_tests() {
    RUN(erase_middle_of_wrapped_cluster_stays_reachable);
    RUN(erase_home_slot_before_wrap_keeps_wrapped_entries);
    RUN(erase_end_of_wrapped_cluster_keeps_earlier_entries);
}

REGISTER_TEST_SUITE();
