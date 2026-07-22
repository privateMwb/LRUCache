// Constructing an LRUCache with zero capacity.
//
// Demonstrates:
// - LRUCache(0) throwing std::invalid_argument
// - Why: a cache that can never hold anything isn't a cache — it's a
//   guaranteed miss on every lookup — so the constructor rejects it
//   outright instead of silently building an unusable one

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    // Capacity must be at least 1 — a precondition, not a soft default
    // the constructor tries to work around.
    setTitle("Zero Capacity Throws");

    try {
        LRUCache<std::string, int> cache(0);
        std::cout << "unreachable\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "caught: " << e.what() << "\n\n";
    }

    // The fix is just picking a real capacity — even 1 is valid, it just
    // means every put() evicts whatever was there before.
    setTitle("Correct Usage");

    LRUCache<std::string, int> cache(1);
    cache.put("only", 1);

    std::cout << "capacity: " << cache.capacity() << "\n";
    std::cout << "size    : " << cache.size() << "\n";
}

REGISTER_EXAMPLE_SUITE();
