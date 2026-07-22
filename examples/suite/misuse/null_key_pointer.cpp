// Dereferencing mostRecentKey()/leastRecentKey() on an empty cache.
//
// Demonstrates:
// - Both returning nullptr when the cache has no entries
// - The mistake, shown but not executed
// - The fix: check for nullptr before dereferencing

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(3);

    // On an empty cache there's no "most recent" or "least recent" key to
    // point at — both functions return nullptr rather than a dangling or
    // default-constructed reference.
    setTitle("nullptr on an Empty Cache");

    std::cout << "mostRecentKey() == nullptr : " << (cache.mostRecentKey() == nullptr) << "\n";
    std::cout << "leastRecentKey() == nullptr: " << (cache.leastRecentKey() == nullptr) << "\n\n";

    // Dereferencing either without checking first is undefined behavior —
    // not executed here for exactly that reason:
    //
    //     std::cout << *cache.mostRecentKey(); // UB: nullptr dereference
    setTitle("The Mistake (not executed)");

    std::cout << "(see comment above — this line is never actually run)\n\n";

    // The correct pattern checks the pointer before using it.
    setTitle("The Fix");

    if (const std::string* key = cache.mostRecentKey()) {
        std::cout << "most recent: " << *key << "\n";
    } else {
        std::cout << "cache is empty — nothing to report\n";
    }
}

REGISTER_EXAMPLE_SUITE();
