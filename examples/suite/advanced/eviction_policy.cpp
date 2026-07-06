// LRUCache eviction policy examples.
//
// Demonstrates:
// - eviction when capacity is exceeded
// - least-recently-used (LRU) replacement
// - promotion through get()
// - predicting the next eviction candidate

#include <common/framework.h>

using namespace CachePro;

static void run_examples() {

    // Demonstrates eviction after exceeding capacity.
    setTitle("Fill To Capacity");

    LRUCache<std::string, int> cache(3);
    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    std::cout << "Size         : " << cache.size() << "\n";
    std::cout << "Least recent : " << *cache.leastRecentKey() << "\n";
    std::cout << "Most recent  : " << *cache.mostRecentKey() << "\n\n";

    // Demonstrates automatic eviction of the LRU entry.
    setTitle("Insert Past Capacity");

    cache.put("d", 4);

    std::cout << "Size after put(\"d\") : " << cache.size() << "\n";
    std::cout << "contains(\"a\")       : " << cache.contains("a") << "  (evicted)\n";
    std::cout << "contains(\"b\")       : " << cache.contains("b") << "\n";
    std::cout << "contains(\"c\")       : " << cache.contains("c") << "\n";
    std::cout << "contains(\"d\")       : " << cache.contains("d") << "\n\n";

    // Demonstrates that get() promotes an entry to MRU.
    setTitle("Promotion Protects From Eviction");

    LRUCache<std::string, int> promo(2);
    promo.put("x", 10);
    promo.put("y", 20);

    (void)promo.get("x");
    promo.put("z", 30);

    std::cout << "contains(\"x\") : " << promo.contains("x") << "  (protected by get)\n";
    std::cout << "contains(\"y\") : " << promo.contains("y") << "  (evicted)\n";
    std::cout << "contains(\"z\") : " << promo.contains("z") << "\n\n";

    // Demonstrates inspection of the next eviction candidate.
    setTitle("Predicting The Next Eviction");

    LRUCache<std::string, int> preview(2);
    preview.put("first", 1);
    preview.put("second", 2);

    std::cout << "leastRecentKey() : " << *preview.leastRecentKey()
              << "  (will be evicted next)\n";
}

REGISTER_EXAMPLE_SUITE();