// Exception safety of LRUCache::emplace().
//
// Demonstrates:
// - A value type whose constructor can throw
// - emplace() building that value in place, directly inside pool memory
// - The cache being left exactly as it was when construction throws
// - The cache remaining fully usable afterward

#include <support/framework.h>

namespace {

// A value type that throws for one specific input — standing in for any
// constructor that can fail (bad input, a validation check, a resource
// that isn't available).
struct RiskyValue {
    int n;

    explicit RiskyValue(int v) : n(v) {
        if (v == 13) {
            throw std::runtime_error("13 is unlucky");
        }
    }
};

} // namespace

using namespace CachePro;

static void run_examples() {

    LRUCache<int, RiskyValue> cache(3);

    // emplace() forwards its arguments straight into RiskyValue's
    // constructor, building the value directly in the node's pool slot —
    // no intermediate temporary.
    setTitle("Normal Emplace");

    cache.emplace(1, 100);
    cache.emplace(2, 200);

    std::cout << "size: " << cache.size() << "\n\n";

    // key 3's value would be RiskyValue(13), which throws. That exception
    // happens mid-construction, inside the node's pool slot — but
    // emplace() only links the node into the table and list *after*
    // construction succeeds, so a throw here leaves the cache exactly as
    // it was: no partially-constructed entry, no leaked slot.
    setTitle("Throwing Constructor");

    try {
        cache.emplace(3, 13);
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "\n";
    }

    std::cout << "size after failed emplace()    : " << cache.size() << "\n";
    std::cout << "contains(3) after failed emplace: " << cache.contains(3) << "\n\n";

    // The cache is untouched by the failure — the next emplace() succeeds
    // normally, reusing the same pool slot the failed attempt never kept.
    setTitle("Cache Still Usable");

    cache.emplace(3, 300);

    std::cout << "size    : " << cache.size() << "\n";
    std::cout << "value(3): " << cache.get(3)->n << "\n";
}

REGISTER_EXAMPLE_SUITE();
