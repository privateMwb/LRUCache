// Exporting LRUCache contents to other containers and formats.
//
// Demonstrates:
// - Snapshotting into a std::map, for stable key-sorted iteration
// - Snapshotting into CSV lines, for writing out to a file or log
// - Doing both with peek(), so the export never disturbs recency order

#include <map>
#include <sstream>
#include <support/framework.h>

using namespace CachePro;

static void run_examples() {

    LRUCache<std::string, int> cache(4);
    cache.put("timeout", 30);
    cache.put("retries", 3);
    cache.put("maxConns", 10);
    cache.put("debug", 0);

    // LRUCache has no ordered iteration of its own — keys() comes back in
    // recency order, not sorted. A std::map snapshot gives sorted,
    // stable iteration for anything that wants to display or diff config
    // by key rather than by recency.
    setTitle("Exporting to std::map");

    std::map<std::string, int> sorted;
    for (const std::string& key : cache.keys()) {
        sorted[key] = *cache.peek(key);
    }

    for (const auto& [key, value] : sorted) {
        std::cout << key << " = " << value << "\n";
    }
    std::cout << "\n";

    // The same walk works for any output format — here, CSV lines
    // suitable for writing straight to a file.
    setTitle("Exporting to CSV Lines");

    std::vector<std::string> lines;
    for (const std::string& key : cache.keys()) {
        std::ostringstream line;
        line << key << "," << *cache.peek(key);
        lines.push_back(line.str());
    }

    for (const std::string& line : lines) {
        std::cout << line << "\n";
    }
}

REGISTER_EXAMPLE_SUITE();
