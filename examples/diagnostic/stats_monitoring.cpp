// LRUCache statistics monitoring examples.
//
// Demonstrates:
// - hit and miss tracking
// - hit rate calculation
// - resetting statistics
// - operations that do not affect statistics

#include <common/framework.h>

using namespace CachePro;

static void run_examples() {

    // Demonstrates recording cache hits and misses.
    setTitle("Recording Hits And Misses");

    LRUCache<std::string, int> cache(3);
    cache.put("session:1", 100);
    cache.put("session:2", 200);

    (void)cache.get("session:1");
    (void)cache.get("session:1");
    (void)cache.get("session:2");
    (void)cache.get("session:99");

    std::cout << "Hits   : " << cache.hitCount() << "\n";
    std::cout << "Misses : " << cache.missCount() << "\n\n";

    // Demonstrates cache hit rate calculation.
    setTitle("Hit Rate");

    std::cout << "Hit rate : " << cache.hitRate() << "%\n\n";

    // Demonstrates operations that do not modify statistics.
    setTitle("Non-Recording Operations");

    (void)cache.peek("session:1");
    (void)cache.contains("session:2");
    cache.put("session:3", 300);

    std::cout << "Hits after peek/contains/put   : " << cache.hitCount() << "  (unchanged)\n";
    std::cout << "Misses after peek/contains/put : " << cache.missCount() << "  (unchanged)\n\n";

    // Demonstrates resetting all statistics counters.
    setTitle("Resetting Stats");

    cache.resetStats();

    std::cout << "Hits after reset     : " << cache.hitCount() << "\n";
    std::cout << "Misses after reset   : " << cache.missCount() << "\n";
    std::cout << "Hit rate after reset : " << cache.hitRate() << "%\n";
}

REGISTER_EXAMPLE_SUITE();