// Resizing an LRUCache down to zero.
//
// Demonstrates:
// - resize(0) throwing std::invalid_argument, the same guard as
//   construction — 0 isn't a valid capacity at any point in the cache's
//   lifetime, not just at construction
// - The cache being left completely untouched when resize() throws

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(3);
    cache.put("a", 1);
    cache.put("b", 2);

    // resize() checks for 0 before doing anything else — nothing about
    // the cache changes on the way to throwing.
    setTitle("resize(0) Throws");

    try {
        cache.resize(0);
        std::cout << "unreachable\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "caught: " << e.what() << "\n\n";
    }

    // Size, capacity, and every entry are exactly what they were before
    // the failed call.
    setTitle("Cache Left Untouched");

    std::cout << "capacity     : " << cache.capacity() << "\n";
    std::cout << "size         : " << cache.size() << "\n";
    std::cout << "contains(\"a\"): " << cache.contains("a") << "\n";
    std::cout << "contains(\"b\"): " << cache.contains("b") << "\n";
}

REGISTER_EXAMPLE_SUITE();
