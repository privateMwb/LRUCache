// LRUCache Construction Benchmark Suite
// Measures LRUCache construction (and immediate destruction) performance
// against stdLRU.
//
// Each timed iteration constructs a fresh, empty cache sized for
// kCapacity entries and lets it go out of scope immediately —
// isolating pool + table allocation (and matching teardown) from any
// put()/get() cost.
//
// Covers:
// - constructing an empty cache with room for kCapacity entries

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures constructing (and destroying) an empty cache.
static void ConstructEmpty_LRUCache(benchmark::State& state) {
    for (auto _ : state) {
        LRUCache<int, int> cache(kCapacity);
        benchmark::DoNotOptimize(cache);
    }
}
BENCHMARK(ConstructEmpty_LRUCache);

static void ConstructEmpty_stdLRU(benchmark::State& state) {
    for (auto _ : state) {
        stdLRU<int, int> cache(kCapacity);
        benchmark::DoNotOptimize(cache);
    }
}
BENCHMARK(ConstructEmpty_stdLRU);
