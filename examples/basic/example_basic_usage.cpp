// Basic LRUCache usage.
//
// Demonstrates:
// - Construction with a fixed capacity
// - put() insertion
// - get() lookup with LRU promotion
// - contains()
// - Capacity and size queries

#include <common/example_helper.h>

using namespace CachePro;

int main() {
    mainTitle("\nBasic Usage Examples");
    borderLine();

    // Constructs a cache with a fixed capacity.
    setTitle("Construction");

    LRUCache<std::string, int> cache(3);

    std::cout << "Capacity : " << cache.capacity() << "\n";
    std::cout << "Size     : " << cache.size() << "\n";
    std::cout << "Empty    : " << cache.empty() << "\n\n";

    // Inserts entries into the cache.
    setTitle("Put");

    cache.put("alpha", 1);
    cache.put("beta", 2);
    cache.put("gamma", 3);

    std::cout << "Size after 3 puts : " << cache.size() << "\n\n";

    // Retrieves entries by key.
    setTitle("Get");

    if (int* value = cache.get("alpha")) {
        std::cout << "get(\"alpha\") : " << *value << "\n";
    }

    if (int* missing = cache.get("missing")) {
        std::cout << "get(\"missing\") : " << *missing << "\n";
    } else {
        std::cout << "get(\"missing\") : not found\n";
    }
    std::cout << "\n";

    // Checks whether keys exist without affecting LRU order.
    setTitle("Contains");

    std::cout << "contains(\"beta\")    : " << cache.contains("beta") << "\n";
    std::cout << "contains(\"unknown\") : " << cache.contains("unknown") << "\n";

    borderLine();
    std::cout << "\n";
    return 0;
}