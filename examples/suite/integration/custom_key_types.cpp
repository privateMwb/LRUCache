// Using a compound key type with LRUCache.
//
// Demonstrates:
// - A struct key with its own operator==
// - Why a custom Hash functor is required: std::hash has no built-in
//   specialization for user-defined types
// - Passing that Hash as LRUCache's third template argument

#include <support/framework.h>

namespace {

// A simple compound key — a grid coordinate, the kind of thing a tile
// cache or spatial index would key on.
struct Point {
    int x;
    int y;
};

bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

// LRUCache's KeyEqual defaults to std::equal_to<K>, which just calls
// operator== above — no extra work needed there. std::hash<Point>,
// though, doesn't exist: the standard library only specializes std::hash
// for built-in and library types, never for a user's own struct. Without
// this, LRUCache<Point, ...> simply wouldn't compile.
struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

} // namespace

using namespace CachePro;

static void run_examples() {

    // The custom hash goes in the third template argument; KeyEqual is
    // left at its default since operator== above already does the job.
    setTitle("Custom Key Type");

    LRUCache<Point, std::string, PointHash> cache(3);

    cache.put(Point{0, 0}, "origin");
    cache.put(Point{1, 1}, "diagonal");
    cache.put(Point{3, -2}, "off-axis");

    std::cout << "size: " << cache.size() << "\n\n";

    // From here the cache behaves exactly like any other — lookups just
    // go through PointHash and operator== instead of the defaults.
    setTitle("Using It");

    std::cout << "get({0, 0})   : " << *cache.get(Point{0, 0}) << "\n";
    std::cout << "contains({1,1}): " << cache.contains(Point{1, 1}) << "\n";
    std::cout << "contains({9,9}): " << cache.contains(Point{9, 9}) << "\n";
}

REGISTER_EXAMPLE_SUITE();
