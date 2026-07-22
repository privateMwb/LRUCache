// LRUCache with the standard algorithms library.
//
// Demonstrates:
// - keys() as a plain std::vector<K>, usable with any STL algorithm
// - std::find() for presence checks that don't need contains()
// - std::ranges::sort() on a snapshot, without touching cache order
// - std::accumulate() and std::count_if() over the snapshot

#include <algorithm>
#include <numeric>
#include <ranges>
#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(5);
    cache.put("apple", 3);
    cache.put("banana", 1);
    cache.put("cherry", 5);
    cache.put("date", 2);
    cache.put("elderberry", 4);

    // keys() is just a std::vector<K> — a snapshot, most recently used
    // first — so every algorithm in <algorithm> already works on it.
    setTitle("A Plain std::vector");

    std::vector<std::string> recent = cache.keys();
    std::cout << "count: " << recent.size() << "\n\n";

    // std::find() over that snapshot is a fine alternative to contains()
    // when you're already holding the vector for other reasons.
    setTitle("std::find()");

    bool hasCherry = std::find(recent.begin(), recent.end(), "cherry") != recent.end();
    std::cout << "found \"cherry\": " << hasCherry << "\n\n";

    // Sorting the snapshot is just sorting a vector — it has no effect on
    // the cache's actual recency order, since keys() already copied it out.
    setTitle("std::ranges::sort()");

    std::ranges::sort(recent);
    for (const std::string& key : recent) {
        std::cout << key << " ";
    }
    std::cout << "\n\n";

    // Aggregating over the cache means looking each key back up — peek()
    // keeps this read-only, same as any other inspection pass.
    setTitle("std::accumulate() / std::count_if()");

    int total =
        std::accumulate(recent.begin(), recent.end(), 0,
                        [&](int sum, const std::string& key) { return sum + *cache.peek(key); });

    long longNames = std::count_if(recent.begin(), recent.end(),
                                   [](const std::string& key) { return key.size() > 5; });

    std::cout << "sum of values      : " << total << "\n";
    std::cout << "keys longer than 5 : " << longNames << "\n";
}

REGISTER_EXAMPLE_SUITE();
