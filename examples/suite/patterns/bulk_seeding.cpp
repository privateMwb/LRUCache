// Bulk-seeding an LRUCache from an existing data source.
//
// Demonstrates:
// - Preloading a cache from a container before serving real traffic
// - Seed order becoming recency order: later entries end up more recent
// - The gotcha: seeding more entries than capacity silently evicts the
//   earliest ones, so only the tail of the seed data survives

#include <support/framework.h>
#include <vector>

using namespace CachePro;

static void run_examples() {

    // A stand-in for data pulled from a file, a database query, or a
    // config — anything you'd want a cache already warm with before the
    // first real request arrives.
    std::vector<std::pair<std::string, int>> seedData{
        {"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5},
    };

    // Seeding is nothing more than put() in a loop, in order.
    setTitle("Seeding");

    LRUCache<std::string, int> cache(3);
    for (const auto& [key, value] : seedData) {
        cache.put(key, value);
    }

    std::cout << "seed entries : " << seedData.size() << "\n";
    std::cout << "cache size   : " << cache.size() << "\n\n";

    // The gotcha: seedData had 5 entries but capacity is 3, so the first
    // two ("a", "b") were evicted before seeding even finished — put()
    // doesn't know it's being used to seed, it evicts exactly like normal.
    setTitle("What Survived");

    std::cout << "contains(\"a\"): " << cache.contains("a") << "\n";
    std::cout << "contains(\"b\"): " << cache.contains("b") << "\n";
    std::cout << "contains(\"e\"): " << cache.contains("e") << "\n\n";

    // Only the last `capacity` entries from the seed data remain, ordered
    // most recently seeded first — "e" was put() last, so it's most recent.
    setTitle("Surviving Order");

    for (const std::string& key : cache.keys()) {
        std::cout << key << "\n";
    }
}

REGISTER_EXAMPLE_SUITE();
