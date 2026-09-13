// LRUCache Construction Benchmark Suite
// Measures LRUCache construction (and immediate destruction) performance
// against stdLRU.
//
// Each timed call constructs a fresh, empty cache sized for kCapacity
// entries and lets it go out of scope immediately — isolating pool +
// table allocation (and matching teardown) from any put()/get() cost.
//
// Covers:
// - constructing an empty cache with room for kCapacity entries

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures constructing (and destroying) an empty cache.
static void bench_construct_empty() {
    auto c = [&] {
        LRUCache<int, int> cache(kCapacity);
        doNotOptimize(cache);
    };

    auto s = [&] {
        stdLRU<int, int> cache(kCapacity);
        doNotOptimize(cache);
    };

    BENCH("construct empty", c, s);
}

// Executes all construction benchmark cases.
static void run_benchmarks() {
    bench_construct_empty();
}

REGISTER_BENCH_SUITE();
