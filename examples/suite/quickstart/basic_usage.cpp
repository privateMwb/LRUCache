// Basic LRUCache usage.
//
// Demonstrates:
// - Constructing a cache with a fixed capacity
// - Inserting entries with put()
// - Retrieving entries with get(), including a miss
// - Capacity, size, and empty checks
// - Clearing a cache

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    // A cache is constructed with a fixed capacity — the maximum number
    // of entries it will ever hold at once.
    setTitle("Construction");

    LRUCache<std::string, int> cache(3);

    std::cout << "capacity: " << cache.capacity() << "\n";
    std::cout << "size    : " << cache.size() << "\n";
    std::cout << "empty   : " << cache.empty() << "\n\n";

    // put() inserts a new key, growing size() by one.
    setTitle("Inserting");

    cache.put("alpha", 1);
    cache.put("beta", 2);
    std::cout << "size after 2 puts: " << cache.size() << "\n";

    cache.put("gamma", 3);
    std::cout << "size after 3 puts: " << cache.size() << "\n\n";

    // get() returns a pointer to the value on a hit, or nullptr on a miss.
    setTitle("Retrieving");

    if (int* value = cache.get("beta")) {
        std::cout << "get(\"beta\")   : " << *value << "\n";
    }

    if (cache.get("missing") == nullptr) {
        std::cout << "get(\"missing\"): not found\n\n";
    }

    // capacity() never changes on its own; size() reflects how many entries
    // are actually stored right now.
    setTitle("Capacity and Size");

    std::cout << "capacity : " << cache.capacity() << "\n";
    std::cout << "size     : " << cache.size() << "\n";

    cache.clear();

    std::cout << "size after clear() : " << cache.size() << "\n";
    std::cout << "empty after clear(): " << cache.empty() << "\n";
    std::cout << "capacity unchanged : " << cache.capacity() << "\n";
}

REGISTER_EXAMPLE_SUITE();
