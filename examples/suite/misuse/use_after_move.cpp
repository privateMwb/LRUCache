// Using an LRUCache after it's been moved from.
//
// Demonstrates:
// - put() throwing cleanly on a moved-from cache
// - Why get()/peek()/contains() are NOT safe here — shown in a comment,
//   not executed, since it's undefined behavior rather than a catchable
//   exception
// - What's actually safe to call on a moved-from cache

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> original(3);
    original.put("alpha", 1);

    LRUCache<std::string, int> moved(std::move(original));
    // original is now moved-from: capacity 0, no pool, no table.

    // put() explicitly checks for exactly this state and throws instead
    // of touching null storage.
    setTitle("put() Throws Cleanly");

    try {
        original.put("beta", 2);
        std::cout << "unreachable\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "caught: " << e.what() << "\n\n";
    }

    // get()/peek()/contains() have no such guard — they compute a table
    // index and dereference it directly, so calling any of them on a
    // moved-from cache is undefined behavior, not a catchable exception.
    // Not executed here for exactly that reason:
    //
    //     original.get("beta");      // UB: dereferences a null table_
    //     original.peek("beta");     // UB: same reason
    //     original.contains("beta"); // UB: same reason
    setTitle("What's Actually Safe");

    std::cout << "capacity: " << original.capacity() << "\n";
    std::cout << "size    : " << original.size() << "\n";
    std::cout << "empty   : " << original.empty() << "\n";
}

REGISTER_EXAMPLE_SUITE();
