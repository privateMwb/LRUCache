// Changing an LRUCache's capacity after construction.
//
// Demonstrates:
// - resize() growing capacity without evicting anything
// - resize() shrinking capacity, evicting least-recently-used entries first
// - reserve() being a documented no-op on this cache
// - shrink_to_fit() physically reclaiming capacity down to size()

#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(2);
    cache.put("a", 1);
    cache.put("b", 2);

    // Growing never evicts — the existing entries and their recency order
    // are untouched, there's just more room now.
    setTitle("resize() Growing");

    cache.resize(4);
    cache.put("c", 3);
    cache.put("d", 4);

    std::cout << "capacity: " << cache.capacity() << "\n";
    std::cout << "size    : " << cache.size() << "\n\n";
    // Order right now, front (most recent) to back (least recent): d, c, b, a.

    // Shrinking below the current size evicts least-recently-used entries
    // first, same as put() would, until size() fits the new capacity.
    setTitle("resize() Shrinking");

    cache.resize(2);

    std::cout << "capacity     : " << cache.capacity() << "\n";
    std::cout << "size         : " << cache.size() << "\n";
    std::cout << "contains(a)  : " << cache.contains("a") << " (evicted first — it was LRU)\n";
    std::cout << "contains(b)  : " << cache.contains("b") << " (evicted second)\n";
    std::cout << "contains(c)  : " << cache.contains("c") << "\n";
    std::cout << "contains(d)  : " << cache.contains("d") << "\n\n";

    // reserve() exists only for API compatibility with map-backed cache
    // implementations that need it. This cache's table is already sized
    // correctly at construction and every resize(), so there's nothing
    // for reserve() to preallocate — it's a documented no-op here.
    setTitle("reserve() Is a No-op");

    cache.reserve(1000);

    std::cout << "capacity unchanged: " << cache.capacity() << "\n";
    std::cout << "size unchanged    : " << cache.size() << "\n\n";

    // Growing capacity well past what's actually stored leaves unused
    // pool and table space behind. shrink_to_fit() physically reclaims
    // it, rebuilding storage down to exactly size() — unlike resize(),
    // it never evicts, since there's nothing beyond the current size to
    // shrink below.
    setTitle("shrink_to_fit()");

    cache.resize(10);
    std::cout << "capacity before shrink_to_fit(): " << cache.capacity() << "\n";

    cache.shrink_to_fit();

    std::cout << "capacity after shrink_to_fit() : " << cache.capacity() << "\n";
    std::cout << "size unchanged throughout      : " << cache.size() << "\n";
}

REGISTER_EXAMPLE_SUITE();
