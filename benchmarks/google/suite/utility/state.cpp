// LRUCache Size/Empty/Capacity Benchmark Suite
// Measures LRUCache's size(), empty(), and capacity() against stdLRU's
// direct equivalents.
//
// Covers:
// - size() (paired against stdLRU)
// - empty() (paired against stdLRU)
// - capacity() (paired against stdLRU)

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures size() against stdLRU's size().
static void Size_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        std::size_t v = cache.size();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(Size_LRUCache);

static void Size_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        std::size_t v = cache.size();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(Size_stdLRU);

// Measures empty() against stdLRU's empty().
static void Empty_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        bool v = cache.empty();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(Empty_LRUCache);

static void Empty_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        bool v = cache.empty();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(Empty_stdLRU);

// Measures capacity() against stdLRU's capacity().
static void Capacity_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);

    for (auto _ : state) {
        std::size_t v = cache.capacity();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(Capacity_LRUCache);

static void Capacity_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);

    for (auto _ : state) {
        std::size_t v = cache.capacity();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(Capacity_stdLRU);
