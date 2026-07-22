// Auditing an LRUCache without disturbing it.
//
// Demonstrates:
// - Using peek() instead of get() for read-only inspection
// - Confirming recency order survives an audit untouched
// - Why get() would be the wrong tool for this: it promotes on every call

#include <support/framework.h>

using namespace CachePro;

namespace {

// A diagnostic pass over every entry — logging, metrics, a health-check
// endpoint. It must not change which entry gets evicted next, so it
// reads with peek(), never get().
void auditContents(const LRUCache<std::string, int>& cache) {
    for (const std::string& key : cache.keys()) {
        std::cout << "  " << key << " = " << *cache.peek(key) << "\n";
    }
}

} // namespace

static void run_examples() {

    LRUCache<std::string, int> cache(3);
    cache.put("alpha", 1);
    cache.put("beta", 2);
    cache.put("gamma", 3);
    // Order right now, front (most recent) to back (least recent):
    // gamma, beta, alpha.

    // Recency order before the audit, captured for comparison afterward.
    setTitle("Before Audit");

    std::cout << "leastRecentKey(): " << *cache.leastRecentKey() << "\n\n";

    // Reading every entry with peek() — none of these calls touch recency.
    setTitle("Audit");

    auditContents(cache);
    std::cout << "\n";

    // Order is unchanged: "alpha" is still the least recently used entry,
    // and would still be the next one evicted.
    setTitle("After Audit");

    std::cout << "leastRecentKey(): " << *cache.leastRecentKey() << " (unchanged)\n\n";

    // For contrast: a single get() on the current LRU entry promotes it,
    // immediately handing the "least recently used" spot to whatever was
    // next in line. peek() above touched every entry without this side
    // effect at all.
    setTitle("Contrast With get()");

    (void)cache.get("alpha");

    std::cout << "leastRecentKey() after one get(): " << *cache.leastRecentKey() << " (changed)\n";
}

REGISTER_EXAMPLE_SUITE();
