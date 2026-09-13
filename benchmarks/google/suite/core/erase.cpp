// LRUCache Erase Benchmark Suite
// Measures LRUCache erase() performance against stdLRU.
//
// Both caches are pre-filled with kCapacity unique keys, sized well
// above the iteration count Google Benchmark is expected to run, so
// every timed erase() removes a real, still-present entry rather than
// hitting the not-found path partway through the run.
//
// Covers:
// - erase() on an existing key

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'200'000;
} // namespace

// Measures erase() removing an existing key.
static void EraseExisting_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }
    int counter = 0;

    for (auto _ : state) {
        bool v = cache.erase(counter);
        benchmark::DoNotOptimize(v);
        ++counter;
    }
}
BENCHMARK(EraseExisting_LRUCache);

static void EraseExisting_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }
    int counter = 0;

    for (auto _ : state) {
        bool v = cache.erase(counter);
        benchmark::DoNotOptimize(v);
        ++counter;
    }
}
BENCHMARK(EraseExisting_stdLRU);
