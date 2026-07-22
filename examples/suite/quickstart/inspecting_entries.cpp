// Inspecting LRUCache entries without disturbing them.
//
// Demonstrates:
// - peek() reading a value without affecting recency
// - get() reading a value and promoting it to most recently used
// - contains() checking presence without affecting recency
// - keys() returning every key, most recently used first

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(3);
    cache.put("alpha", 1);
    cache.put("beta", 2);
    cache.put("gamma", 3);
    // Order right now, front (most recent) to back (least recent):
    // gamma, beta, alpha.

    // peek() reads a value like get(), but never changes recency order —
    // useful for inspection code that shouldn't affect what gets evicted.
    setTitle("peek() vs get()");

    std::cout << "leastRecentKey() before peek: " << *cache.leastRecentKey() << "\n";

    std::cout << "peek(\"alpha\")               : " << *cache.peek("alpha") << "\n";
    std::cout << "leastRecentKey() after peek : " << *cache.leastRecentKey() << "\n\n";

    std::cout << "get(\"alpha\")                : " << *cache.get("alpha") << "\n";
    std::cout << "leastRecentKey() after get  : " << *cache.leastRecentKey() << "\n";
    std::cout << "mostRecentKey() after get   : " << *cache.mostRecentKey() << "\n\n";

    // contains() checks presence only — also does not affect recency order.
    setTitle("contains()");

    std::cout << "contains(\"gamma\")  : " << cache.contains("gamma") << "\n";
    std::cout << "contains(\"missing\"): " << cache.contains("missing") << "\n\n";

    // keys() snapshots every key currently stored, ordered most recently
    // used first — the same order the cache would evict from the back of.
    setTitle("keys()");

    for (const std::string& key : cache.keys()) {
        std::cout << key << "\n";
    }
}

REGISTER_EXAMPLE_SUITE();
