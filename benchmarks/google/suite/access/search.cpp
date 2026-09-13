// LRUCache Search Benchmark Suite
// Measures LRUCache lookup-miss performance against stdLRU.
//
// Each case builds its source cache once, outside the timed loop —
// only the repeated miss lookup is measured. The probed key is never
// inserted, so every call walks the full miss path.
//
// Covers:
// - contains() on a missing key
// - get() on a missing key

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
constexpr int kMissingKey = -1; // never inserted below
} // namespace

// Measures contains() on a key that is never present.
static void ContainsMiss_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        bool v = cache.contains(kMissingKey);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(ContainsMiss_LRUCache);

static void ContainsMiss_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        bool v = cache.contains(kMissingKey);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(ContainsMiss_stdLRU);

// Measures get() on a key that is never present.
static void GetMiss_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        int* v = cache.get(kMissingKey);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(GetMiss_LRUCache);

static void GetMiss_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        int* v = cache.get(kMissingKey);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(GetMiss_stdLRU);
