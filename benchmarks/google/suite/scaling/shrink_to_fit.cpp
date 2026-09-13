// LRUCache Shrink-to-Fit Benchmark Suite
// Measures LRUCache shrink_to_fit() performance against stdLRU.
//
// Each iteration adds a bounded, cycling amount of slack then reclaims
// it, so per-call cost stays bounded across the whole run.

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kSeedCount = 100;
constexpr std::size_t kMaxGrowth = 64;
} // namespace

static void ShrinkToFit_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kSeedCount);
    for (int i = 0; i < static_cast<int>(kSeedCount); ++i) {
        cache.put(i, i);
    }
    std::size_t growth = 1;

    for (auto _ : state) {
        std::size_t g = (growth++ % kMaxGrowth) + 1;
        cache.resize(kSeedCount + g); // add slack
        cache.shrink_to_fit();        // reclaim it
    }
}
BENCHMARK(ShrinkToFit_LRUCache);

static void ShrinkToFit_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kSeedCount);
    for (int i = 0; i < static_cast<int>(kSeedCount); ++i) {
        cache.put(i, i);
    }
    std::size_t growth = 1;

    for (auto _ : state) {
        std::size_t g = (growth++ % kMaxGrowth) + 1;
        cache.resize(kSeedCount + g);
        cache.resize(kSeedCount);
    }
}
BENCHMARK(ShrinkToFit_stdLRU);
