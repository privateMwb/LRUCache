// LRUCache move semantics examples.
//
// Demonstrates:
// - move construction
// - move assignment
// - moved-from object state
// - returning a cache from a function

#include <common/framework.h>

using namespace CachePro;

// Builds and returns a populated cache.
LRUCache<std::string, int> buildSessionCache() {
    LRUCache<std::string, int> cache(3);
    cache.put("session:1", 100);
    cache.put("session:2", 200);
    return cache;
}

static void run_examples() {
    
    // Demonstrates move construction.
    setTitle("Move Construction");

    LRUCache<std::string, int> source(3);
    source.put("a", 1);
    source.put("b", 2);

    LRUCache<std::string, int> moved(std::move(source));

    std::cout << "moved.size()  : " << moved.size() << "\n";
    std::cout << "source.size() : " << source.size() << "  (moved-from, now empty)\n\n";

    // Demonstrates move assignment.
    setTitle("Move Assignment");

    LRUCache<std::string, int> target(2);
    target.put("old", 999);

    LRUCache<std::string, int> incoming(3);
    incoming.put("new", 1);

    target = std::move(incoming);

    std::cout << "target.contains(\"old\") : " << target.contains("old") << "  (replaced)\n";
    std::cout << "target.contains(\"new\") : " << target.contains("new") << "\n\n";

    // Demonstrates returning a cache from a function.
    setTitle("Returning A Cache From A Function");

    LRUCache<std::string, int> sessions = buildSessionCache();

    std::cout << "sessions.size()           : " << sessions.size() << "\n";
    std::cout << "sessions.get(\"session:1\") : " << *sessions.get("session:1") << "\n";
}

REGISTER_EXAMPLE_SUITE();