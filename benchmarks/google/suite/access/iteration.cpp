// LRUCache Iteration Benchmark Suite
// Measures LRUCache keys() traversal performance against stdLRU.
//
// Each case builds its source cache once, outside the timed loop —
// only the repeated full traversal is measured. Capacity is kept small
// since each timed call is O(capacity), not O(1).
//
// Covers:
// - keys() — full recency-ordered key traversal

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures a full keys() traversal of a populated, full cache.
static void KeysTraversal_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        auto v = cache.keys();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(KeysTraversal_LRUCache);

static void KeysTraversal_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        auto v = cache.keys();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(KeysTraversal_stdLRU);
