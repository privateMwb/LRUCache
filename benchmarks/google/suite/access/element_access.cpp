// LRUCache Element Access Benchmark Suite
// Measures LRUCache get()/peek() performance against a std-based reference LRU.
//
// Each case builds its source cache once, outside the timed loop —
// only repeated access into an already-full cache is measured.
//
// Covers:
// - get() on an existing key (recency-updating hit path)
// - peek() on an existing key (recency-neutral hit path)

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures get() on an existing key — updates recency order on every call.
static void GetHit_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        int* v = cache.get(0);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(GetHit_LRUCache);

static void GetHit_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        int* v = cache.get(0);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(GetHit_stdLRU);

// Measures peek() on an existing key — does not affect recency order.
static void PeekHit_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        const int* v = cache.peek(0);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(PeekHit_LRUCache);

static void PeekHit_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        const int* v = cache.peek(0);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(PeekHit_stdLRU);
