// Cache-aside pattern with LRUCache.
//
// Demonstrates:
// - Checking the cache first with get()
// - Falling back to a slower source on a miss, then caching the result
// - Repeated lookups of the same key becoming hits
// - Reading hitCount() / missCount() to see the pattern pay off

#include <support/framework.h>

using namespace CachePro;

namespace {

// Stands in for a slow source of truth — a database call, a network
// fetch, a disk read. Cache-aside exists to avoid calling this more
// than necessary.
std::string loadFromSource(int id) {
    return "record-" + std::to_string(id);
}

// The pattern itself: ask the cache first, only fall back to the slow
// path on a miss, and cache whatever the slow path returns so the next
// lookup for the same key is a hit.
std::string fetch(LRUCache<int, std::string>& cache, int id) {
    if (std::string* cached = cache.get(id)) {
        return *cached;
    }

    std::string value = loadFromSource(id);
    cache.put(id, value);
    return value;
}

} // namespace

static void run_examples() {

    LRUCache<int, std::string> cache(3);

    // First look-up for each id is always a miss — nothing has been
    // loaded yet, so fetch() falls through to loadFromSource().
    setTitle("First Lookups (misses)");

    std::cout << "fetch(1): " << fetch(cache, 1) << "\n";
    std::cout << "fetch(2): " << fetch(cache, 2) << "\n";
    std::cout << "fetch(3): " << fetch(cache, 3) << "\n\n";

    // Asking for the same ids again hits the cache instead of calling
    // loadFromSource() a second time.
    setTitle("Repeated Lookups (hits)");

    std::cout << "fetch(1): " << fetch(cache, 1) << "\n";
    std::cout << "fetch(2): " << fetch(cache, 2) << "\n\n";

    // hitCount()/missCount() confirm what actually happened under the
    // hood: 3 misses from the cold start, 2 hits from the repeats.
    setTitle("Stats");

    std::cout << "hitCount()  : " << cache.hitCount() << "\n";
    std::cout << "missCount() : " << cache.missCount() << "\n";
    std::cout << "hitRate()   : " << cache.hitRate() << "%\n";
}

REGISTER_EXAMPLE_SUITE();
