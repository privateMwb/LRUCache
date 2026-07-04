// LRUCache custom type examples.
//
// Demonstrates:
// - string keys
// - move-only value types
// - non-default-constructible value types
// - user-defined value types

#include <common/example_helper.h>

#include <memory>

using namespace CachePro;

// A value type without a default constructor.
struct Config {
    int level;
    std::string label;

    Config(int lvl, std::string lbl)
        : level(lvl), label(std::move(lbl)) {}
};

int main() {
    mainTitle("\nCustom Type Examples");
    borderLine();

    // Demonstrates string keys with primitive values.
    setTitle("String Keys");

    LRUCache<std::string, double> prices(3);
    prices.put("apple", 1.50);
    prices.put("banana", 0.75);

    std::cout << "get(\"apple\")  : " << *prices.get("apple") << "\n";
    std::cout << "get(\"banana\") : " << *prices.get("banana") << "\n\n";

    // Demonstrates support for move-only value types.
    setTitle("Move-Only Values");

    LRUCache<int, std::unique_ptr<int>> owned(3);
    owned.put(1, std::make_unique<int>(42));

    std::cout << "*get(1) : " << **owned.get(1) << "\n";

    owned.put(2, std::make_unique<int>(7));
    std::cout << "*get(2) : " << **owned.get(2) << "\n\n";

    // Demonstrates support for non-default-constructible value types.
    setTitle("Non-Default-Constructible Values");

    LRUCache<int, Config> settings(3);
    settings.put(1, Config(5, "high"));
    settings.emplace(2, 1, "low");

    std::cout << "get(1) : level=" << settings.get(1)->level
              << ", label=" << settings.get(1)->label << "\n";
    std::cout << "get(2) : level=" << settings.get(2)->level
              << ", label=" << settings.get(2)->label << "\n";

    borderLine();
    std::cout << "\n";
    return 0;
}