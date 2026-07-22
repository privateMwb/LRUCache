// Holding a pointer across a call that can reallocate storage.
//
// Demonstrates:
// - get()/peek() pointers becoming dangling after resize() or
//   shrink_to_fit(), either of which can reallocate the pool
// - The mistake, shown but not executed
// - The fix: re-fetch the pointer after any call that might reallocate

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(2);
    cache.put("alpha", 1);
    cache.put("beta", 2);

    // resize() and shrink_to_fit() can both reallocate the pool — every
    // live node gets moved to a new address as part of that. A pointer
    // into the old storage is left dangling the moment it happens.
    setTitle("The Mistake (not executed)");

    // int* value = cache.get("alpha");
    // cache.resize(10);             // may reallocate — "alpha"'s node can move
    // std::cout << *value << "\n";  // UB: value may point at freed memory

    std::cout << "(see comments above — this pattern is never actually run)\n\n";

    // The fix costs nothing: look the value up again after the call that
    // might have reallocated, instead of trusting the old pointer.
    setTitle("The Fix");

    cache.resize(10);
    int* value = cache.get("alpha");

    std::cout << "value after resize: " << *value << "\n";
}

REGISTER_EXAMPLE_SUITE();
