// LRUCache Observer Benchmark Suite
// Measures LRUCache's introspection API that has no stdLRU equivalent —
// stdLRU tracks none of this bookkeeping, so every case here is
// LRUCache-only. size(), empty(), and capacity() have a direct stdLRU
// equivalent and live in state.cpp instead.
//
// Covers:
// - hitCount(), missCount(), hitRate()
// - mostRecentKey(), leastRecentKey()

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>

#include <cstddef>

using namespace CachePro;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures hitCount() — no stdLRU equivalent.
static void HitCount(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cache.put(i, i);
    (void)cache.get(0);
    (void)cache.get(-1);

    for (auto _ : state) {
        std::size_t v = cache.hitCount();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(HitCount);

// Measures missCount() — no stdLRU equivalent.
static void MissCount(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cache.put(i, i);
    (void)cache.get(0);
    (void)cache.get(-1);

    for (auto _ : state) {
        std::size_t v = cache.missCount();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(MissCount);

// Measures hitRate() — no stdLRU equivalent.
static void HitRate(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cache.put(i, i);
    (void)cache.get(0);
    (void)cache.get(-1);

    for (auto _ : state) {
        double v = cache.hitRate();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(HitRate);

// Measures mostRecentKey() — no stdLRU equivalent.
static void MostRecentKey(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cache.put(i, i);

    for (auto _ : state) {
        const int* v = cache.mostRecentKey();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(MostRecentKey);

// Measures leastRecentKey() — no stdLRU equivalent.
static void LeastRecentKey(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i)
        cache.put(i, i);

    for (auto _ : state) {
        const int* v = cache.leastRecentKey();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(LeastRecentKey);
