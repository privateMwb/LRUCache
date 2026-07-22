// Using hit-rate stats to size an LRUCache.
//
// Demonstrates:
// - hitCount() / missCount() / hitRate() under a too-small capacity
// - Recognizing thrashing: a working set larger than capacity evicts
//   entries before they're ever reused
// - resize() to grow the cache, then resetStats() for a clean re-measure
// - The same workload's hit rate improving once capacity fits it

#include <support/framework.h>

using namespace CachePro;

namespace {

// A repeating access pattern over 4 distinct ids — a realistic stand-in
// for "the same handful of records get requested over and over."
void runWorkload(LRUCache<int, std::string>& cache) {
    for (int pass = 0; pass < 2; ++pass) {
        for (int id = 1; id <= 4; ++id) {
            if (cache.get(id) == nullptr) {
                cache.put(id, "record-" + std::to_string(id));
            }
        }
    }
}

} // namespace

static void run_examples() {

    // Capacity 2 can't hold all 4 ids the workload cycles through, so by
    // the time id 1 comes back around it's long since been evicted.
    setTitle("Undersized Capacity");

    LRUCache<int, std::string> cache(2);
    runWorkload(cache);

    std::cout << "hitCount()  : " << cache.hitCount() << "\n";
    std::cout << "missCount() : " << cache.missCount() << "\n";
    std::cout << "hitRate()   : " << cache.hitRate() << "%\n\n";

    // A 0% hit rate here is a signal the cache is thrashing rather than
    // caching. resize() grows it to fit the whole working set, and
    // resetStats() clears the misleading numbers from the undersized run.
    setTitle("Resizing");

    cache.resize(4);
    cache.resetStats();

    std::cout << "capacity after resize: " << cache.capacity() << "\n\n";

    // Re-running the identical workload now finds every id still resident
    // on its second pass — hits instead of evictions.
    setTitle("Re-measuring");

    runWorkload(cache);

    std::cout << "hitCount()  : " << cache.hitCount() << "\n";
    std::cout << "missCount() : " << cache.missCount() << "\n";
    std::cout << "hitRate()   : " << cache.hitRate() << "%\n";
}

REGISTER_EXAMPLE_SUITE();
