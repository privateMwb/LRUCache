// LRUCache update and promotion examples.
//
// Demonstrates:
// - updating existing entries with put()
// - promotion on update
// - in-place construction with emplace()
// - updating existing entries with emplace()

#include <common/framework.h>

using namespace CachePro;

static void run_examples() {

    // Demonstrates updating an existing value with put().
    setTitle("Put Updates Existing Value");

    LRUCache<std::string, int> cache(3);
    cache.put("score", 10);
    std::cout << "score before : " << *cache.get("score") << "\n";

    cache.put("score", 99);
    std::cout << "score after  : " << *cache.get("score") << "\n";
    std::cout << "size         : " << cache.size() << "  (unchanged)\n\n";

    // Demonstrates promotion after updating an existing key.
    setTitle("Put Promotes On Update");

    LRUCache<std::string, int> order(3);
    order.put("a", 1);
    order.put("b", 2);
    order.put("c", 3);

    std::cout << "most recent before update : " << *order.mostRecentKey() << "\n";

    order.put("a", 111);

    std::cout << "most recent after update  : " << *order.mostRecentKey() << "\n\n";

    // Demonstrates in-place value construction with emplace().
    setTitle("Emplace Constructs In Place");

    LRUCache<int, std::string> names(3);
    std::string& built = names.emplace(1, "constructed in place");

    std::cout << "emplace return value : " << built << "\n";
    std::cout << "get(1)               : " << *names.get(1) << "\n\n";

    // Demonstrates updating and promoting an existing entry with emplace().
    setTitle("Emplace Updates And Promotes");

    LRUCache<int, std::string> tags(3);
    tags.put(1, "one");
    tags.put(2, "two");
    tags.put(3, "three");

    tags.emplace(1, "ONE-UPDATED");

    std::cout << "get(1)          : " << *tags.get(1) << "\n";
    std::cout << "most recent key : " << *tags.mostRecentKey() << "\n";
}

REGISTER_EXAMPLE_SUITE();