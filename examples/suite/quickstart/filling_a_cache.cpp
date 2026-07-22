// Filling an LRUCache to capacity.
//
// Demonstrates:
// - size() reaching capacity()
// - Updating an existing key in place, without growing size()
// - The move-taking put() overload
// - Eviction of the least recently used entry once the cache is full

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(3);

    // Filling a cache is just put() until size() reaches capacity() — after
    // that, every further put() has to make room for itself.
    setTitle("Filling to Capacity");

    cache.put("alpha", 1);
    cache.put("beta", 2);
    cache.put("gamma", 3);

    std::cout << "size     : " << cache.size() << "\n";
    std::cout << "capacity : " << cache.capacity() << "\n\n";

    // Putting a key that already exists replaces its value in place — size()
    // doesn't grow — and marks it most recently used, same as a hit on get().
    setTitle("Updating In Place");

    cache.put("alpha", 100);

    std::cout << "size unchanged : " << cache.size() << "\n";
    std::cout << "alpha's value  : " << *cache.get("alpha") << "\n\n";

    // put() has four overloads so an rvalue key or value takes the move path
    // instead of copying. Current order, front to back: alpha, gamma, beta.
    setTitle("Move Overload");

    std::string movedKey = "delta";
    cache.put(std::move(movedKey), 4); // cache is full — evicts "beta", the LRU entry

    std::cout << "movedKey after move: \"" << movedKey
              << "\"\n"; // left in a valid, unspecified state
    std::cout << "contains(\"beta\")  : " << cache.contains("beta") << "\n\n";

    // Order is now: delta, alpha, gamma. Inserting once more with the cache
    // still full evicts "gamma", the current least recently used entry.
    setTitle("Eviction");

    std::cout << "before: contains(\"gamma\")   = " << cache.contains("gamma") << "\n";

    cache.put("epsilon", 5);

    std::cout << "after : contains(\"gamma\")   = " << cache.contains("gamma") << "\n";
    std::cout << "after : contains(\"epsilon\") = " << cache.contains("epsilon") << "\n";
    std::cout << "size still at capacity     : " << cache.size() << "\n";
}

REGISTER_EXAMPLE_SUITE();
