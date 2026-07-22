// The pool and free-list design behind LRUCache, observed from outside.
//
// Demonstrates:
// - capacity() staying fixed no matter how much churn put() sees
// - erase() returning its slot for immediate reuse, not deallocation
// - Why none of this needs (or exposes) the pool, free list, or table
//   directly — every effect below is visible through the public API alone

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    // Entries live in a single pool allocated once at construction (or
    // resize()), reused via an embedded free list. Evicting a node
    // doesn't free memory — it just returns that slot to the free list
    // for the very next put() to reuse. Driving thousands of insertions
    // through a capacity-3 cache should still leave capacity() at exactly
    // 3: nothing ever grows to keep up.
    setTitle("Fixed Footprint Under Heavy Churn");

    LRUCache<int, int> cache(3);
    for (int i = 0; i < 10'000; ++i) {
        cache.put(i, i);
    }

    std::cout << "capacity after 10,000 puts: " << cache.capacity() << "\n";
    std::cout << "size after 10,000 puts    : " << cache.size() << "\n\n";

    // erase() works the same way as eviction: destroy the node, return its
    // slot to the free list. The next put() reuses that exact slot rather
    // than growing the pool, so size() drops and climbs back without
    // capacity() ever moving.
    setTitle("erase() Frees a Slot for Reuse");

    bool removed = cache.erase(9999);
    std::cout << "erased key 9999   : " << removed << "\n";
    std::cout << "size after erase  : " << cache.size() << "\n";

    cache.put(20000, 1);

    std::cout << "size after new put: " << cache.size() << "\n";
    std::cout << "capacity unchanged: " << cache.capacity() << "\n";
}

REGISTER_EXAMPLE_SUITE();
