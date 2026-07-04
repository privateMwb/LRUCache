// LRUCache resize examples.
//
// Demonstrates:
// - growing capacity
// - shrinking capacity
// - resizing to the current size
// - rejecting invalid capacity values

#include <common/example_helper.h>

using namespace CachePro;

int main() {
    mainTitle("\nResize Examples");
    borderLine();

    // Demonstrates growing the cache capacity.
    setTitle("Growing Capacity");

    LRUCache<std::string, int> cache(2);
    cache.put("a", 1);
    cache.put("b", 2);

    std::cout << "Capacity before : " << cache.capacity() << "\n";

    cache.resize(5);

    std::cout << "Capacity after  : " << cache.capacity() << "\n";
    std::cout << "Size after      : " << cache.size() << "  (entries preserved)\n\n";

    cache.put("c", 3);
    cache.put("d", 4);

    std::cout << "contains(\"a\") : " << cache.contains("a") << "\n";
    std::cout << "Size           : " << cache.size() << "\n\n";

    // Demonstrates shrinking the cache capacity.
    setTitle("Shrinking Capacity");

    LRUCache<std::string, int> shrinking(4);
    shrinking.put("w", 1);
    shrinking.put("x", 2);
    shrinking.put("y", 3);
    shrinking.put("z", 4);

    std::cout << "Least recent before shrink : " << *shrinking.leastRecentKey() << "\n";

    shrinking.resize(2);

    std::cout << "Capacity after shrink : " << shrinking.capacity() << "\n";
    std::cout << "Size after shrink     : " << shrinking.size() << "\n";
    std::cout << "contains(\"w\")        : " << shrinking.contains("w") << "  (evicted)\n";
    std::cout << "contains(\"z\")        : " << shrinking.contains("z") << "  (kept, most recent)\n\n";

    // Demonstrates resizing to the current number of entries.
    setTitle("Shrink To Current Size");

    LRUCache<std::string, int> exact(4);
    exact.put("m", 1);
    exact.put("n", 2);

    exact.resize(2);

    std::cout << "Size after resize(2) : " << exact.size() << "  (no eviction, size == new capacity)\n\n";

    // Demonstrates validation of invalid resize requests.
    setTitle("Rejecting Invalid Resize");

    try {
        cache.resize(0);
    } catch (const std::invalid_argument& e) {
        std::cout << "resize(0) threw : " << e.what() << "\n";
    }

    borderLine();
    std::cout << "\n";
    return 0;
}