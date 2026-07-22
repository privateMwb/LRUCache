// LRUCache Iteration Benchmark Suite
// Measures LRUCache keys() traversal performance against stdLRU.
//
// Each case builds its source cache once, outside the timed lambda —
// only the repeated full traversal is measured. Capacity is kept small
// since each timed call is O(capacity), not O(1).
//
// Covers:
// - keys() — full recency-ordered key traversal

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures a full keys() traversal of a populated, full cache.
static void bench_keys_traversal() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        auto v = cSrc.keys();
        doNotOptimize(v);
    };

    auto s = [&] {
        auto v = sSrc.keys();
        doNotOptimize(v);
    };

    BENCH("keys() traversal", c, s);
}

// Executes all iteration benchmark cases.
static void run_benchmarks() {
    bench_keys_traversal();
}

REGISTER_BENCH_SUITE();
