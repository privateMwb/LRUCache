// LRUCache order inspection examples.
//
// Demonstrates:
// - listing keys by recency
// - inspecting boundary entries
// - order changes after access
// - inspecting an empty cache

#include <common/framework.h>

using namespace CachePro;

static void run_examples() {

    // Demonstrates listing keys from most- to least-recently-used.
    setTitle("Listing Keys In Recency Order");

    LRUCache<std::string, int> cache(4);
    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    std::cout << "Keys (MRU -> LRU) : ";
    for (const auto& key : cache.keys()) {
        std::cout << key << ' ';
    }
    std::cout << "\n\n";

    // Demonstrates inspecting the most- and least-recently-used entries.
    setTitle("Boundary Keys");

    std::cout << "Most recent  : " << *cache.mostRecentKey() << "\n";
    std::cout << "Least recent : " << *cache.leastRecentKey() << "\n\n";

    // Demonstrates how get() changes the recency order.
    setTitle("Order After Promotion");

    (void)cache.get("a");

    std::cout << "Keys after get(\"a\") : ";
    for (const auto& key : cache.keys()) {
        std::cout << key << ' ';
    }
    std::cout << "\n";
    std::cout << "Least recent now    : " << *cache.leastRecentKey() << "\n\n";

    // Demonstrates inspecting an empty cache.
    setTitle("Inspecting An Empty Cache");

    LRUCache<std::string, int> empty(2);

    std::cout << "keys().size()    : " << empty.keys().size() << "\n";
    std::cout << "mostRecentKey()  : " << (empty.mostRecentKey() ? "found" : "nullptr") << "\n";
    std::cout << "leastRecentKey() : " << (empty.leastRecentKey() ? "found" : "nullptr") << "\n";
}

REGISTER_EXAMPLE_SUITE();