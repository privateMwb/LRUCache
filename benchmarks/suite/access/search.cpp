// LRUCache Search Benchmark Suite
// Measures LRUCache lookup-miss performance against stdLRU.
//
// Each case builds its source cache once, outside the timed lambda —
// only the repeated miss lookup is measured. The probed key is never
// inserted, so every call walks the full miss path.
//
// Covers:
// - contains() on a missing key
// - get() on a missing key

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
constexpr int kMissingKey = -1; // never inserted below
} // namespace

// Measures contains() on a key that is never present.
static void bench_contains_miss() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        bool v = cSrc.contains(kMissingKey);
        doNotOptimize(v);
    };

    auto s = [&] {
        bool v = sSrc.contains(kMissingKey);
        doNotOptimize(v);
    };

    BENCH("contains() miss", c, s);
}

// Measures get() on a key that is never present.
static void bench_get_miss() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        int* v = cSrc.get(kMissingKey);
        doNotOptimize(v);
    };

    auto s = [&] {
        int* v = sSrc.get(kMissingKey);
        doNotOptimize(v);
    };

    BENCH("get() miss", c, s);
}

// Executes all search benchmark cases.
static void run_benchmarks() {
    bench_contains_miss();
    std::cout << "\n";

    bench_get_miss();
}

REGISTER_BENCH_SUITE();
