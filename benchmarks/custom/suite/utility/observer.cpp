// LRUCache Observer Benchmark Suite
// Measures LRUCache's introspection API that has no stdLRU equivalent —
// stdLRU tracks none of this bookkeeping, so every case here runs
// through BENCH_SOLO. size(), empty(), and capacity() have a direct
// stdLRU equivalent and live in size_empty_capacity.cpp instead.
//
// Covers:
// - hitCount(), missCount(), hitRate()
// - mostRecentKey(), leastRecentKey()

#include <support/framework.h>

using namespace CachePro;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures hitCount() / missCount() / hitRate() — no stdLRU equivalent.
static void bench_stats_solo() {
    LRUCache<int, int> cSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cSrc.put(i, i);
    (void)cSrc.get(0);
    (void)cSrc.get(-1);

    auto hitCount = [&] {
        std::size_t v = cSrc.hitCount();
        doNotOptimize(v);
    };
    BENCH_SOLO("hitCount()", hitCount);

    std::cout << "\n";

    auto missCount = [&] {
        std::size_t v = cSrc.missCount();
        doNotOptimize(v);
    };
    BENCH_SOLO("missCount()", missCount);

    std::cout << "\n";

    auto hitRate = [&] {
        double v = cSrc.hitRate();
        doNotOptimize(v);
    };
    BENCH_SOLO("hitRate()", hitRate);
}

// Measures mostRecentKey() / leastRecentKey() — no stdLRU equivalent.
static void bench_recency_keys_solo() {
    LRUCache<int, int> cSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cSrc.put(i, i);

    auto mostRecent = [&] {
        const int* v = cSrc.mostRecentKey();
        doNotOptimize(v);
    };
    BENCH_SOLO("mostRecentKey()", mostRecent);

    std::cout << "\n";

    auto leastRecent = [&] {
        const int* v = cSrc.leastRecentKey();
        doNotOptimize(v);
    };
    BENCH_SOLO("leastRecentKey()", leastRecent);
}

// Executes all observer benchmark cases.
static void run_benchmarks() {
    bench_stats_solo();
    std::cout << "\n";

    bench_recency_keys_solo();
}

REGISTER_BENCH_SUITE();
