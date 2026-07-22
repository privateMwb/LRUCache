// Move semantics for LRUCache.
//
// Demonstrates:
// - Move construction transferring ownership of the pool and table
// - Move assignment replacing an existing cache's contents
// - What's actually safe to call on a moved-from cache

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    // Moving a cache transfers its pool and table allocations directly —
    // no entries are copied or rehashed.
    setTitle("Move Construction");

    LRUCache<std::string, int> original(3);
    original.put("alpha", 1);
    original.put("beta", 2);

    LRUCache<std::string, int> moved(std::move(original));

    std::cout << "moved.size()            : " << moved.size() << "\n";
    std::cout << "moved.contains(\"alpha\") : " << moved.contains("alpha") << "\n";
    std::cout << "original.capacity()     : " << original.capacity() << "\n";
    std::cout << "original.size()         : " << original.size() << "\n";
    std::cout << "original.empty()        : " << original.empty() << "\n\n";

    // A moved-from cache has capacity 0 and no storage. put() checks for
    // exactly that and throws rather than touching null storage — but
    // that guard only exists on put(). get()/peek()/contains() don't
    // check, so calling any of them on a moved-from cache is undefined
    // behavior, not a clean exception. Only capacity(), size(), empty(),
    // and put() (via its exception) are safe to call here.
    setTitle("Moved-From State");

    try {
        original.put("gamma", 3);
    } catch (const std::exception& e) {
        std::cout << "put() on moved-from cache threw: " << e.what() << "\n\n";
    }

    // Move assignment destroys the target's existing contents first, then
    // takes ownership of the source's pool and table — same end state as
    // move construction, just onto an already-live cache.
    setTitle("Move Assignment");

    LRUCache<std::string, int> a(2);
    a.put("x", 10);
    a.put("y", 20);

    LRUCache<std::string, int> b(5);
    b.put("z", 99);

    b = std::move(a);

    std::cout << "b.size()         : " << b.size() << "\n";
    std::cout << "b.capacity()     : " << b.capacity() << "\n";
    std::cout << "b.contains(\"z\")  : " << b.contains("z") << " (destroyed by the assignment)\n";
    std::cout << "b.contains(\"x\")  : " << b.contains("x") << "\n";
    std::cout << "a.size()         : " << a.size() << " (moved-from)\n";
}

REGISTER_EXAMPLE_SUITE();
