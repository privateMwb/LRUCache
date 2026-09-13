// LRUCache Reserve Benchmark Suite
// Measures LRUCache reserve() performance against stdLRU.
//
// LRUCache's table is sized once for `capacity_` at construction and
// never grows on its own, so reserve() is a documented no-op — kept
// only for API compatibility with map-backed cache implementations.
// stdLRU's reserve() forwards to the real
// std::unordered_map::reserve(). The near-zero-cost result on the
// LRUCache side is the point of this comparison, not a flaw in it.
//
// Covers:
// - reserve() on an already-constructed cache

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
constexpr std::size_t kReserveCount = 1'000;
} // namespace

// Measures reserve() on a cache already sized for kCapacity entries.
static void Reserve_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);

    for (auto _ : state) {
        cache.reserve(kReserveCount);
    }
}
BENCHMARK(Reserve_LRUCache);

static void Reserve_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);

    for (auto _ : state) {
        cache.reserve(kReserveCount);
    }
}
BENCHMARK(Reserve_stdLRU);
