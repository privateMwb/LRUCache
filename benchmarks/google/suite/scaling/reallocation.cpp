// LRUCache Reallocation Benchmark Suite
// Measures LRUCache resize() growing performance against stdLRU.
//
// Each timed iteration grows capacity by one entry over the last, so
// every call does real pool + table rebuild work (LRUCache) or an
// implicit rehash pass (stdLRU) rather than hitting a no-op "already
// big enough" path. The cache is seeded with a small live population
// so the per-call rebuild walks real, still-linked entries.
//
// Covers:
// - resize() growing capacity by one entry, repeated

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kSeedCount = 100;
constexpr std::size_t kInitialCapacity = 100;
} // namespace

// Measures resize() growing capacity by one entry on every call.
static void ResizeGrow_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kInitialCapacity);
    for (int i = 0; i < static_cast<int>(kSeedCount); ++i) {
        cache.put(i, i);
    }
    std::size_t growth = 0;

    for (auto _ : state) {
        cache.resize(kInitialCapacity + ++growth);
    }
}
BENCHMARK(ResizeGrow_LRUCache);

static void ResizeGrow_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kInitialCapacity);
    for (int i = 0; i < static_cast<int>(kSeedCount); ++i) {
        cache.put(i, i);
    }
    std::size_t growth = 0;

    for (auto _ : state) {
        cache.resize(kInitialCapacity + ++growth);
    }
}
BENCHMARK(ResizeGrow_stdLRU);
