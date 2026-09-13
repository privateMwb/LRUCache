// LRUCache Clear-and-Refill Benchmark Suite
// Measures LRUCache clear() + refill performance against stdLRU.
//
// Each timed iteration clears a full cache back to empty, then refills
// it to capacity again — a full teardown/rebuild cycle. Capacity is
// kept small since each iteration does O(capacity) work, not O(1).
//
// Covers:
// - clear() on a full cache, then refill from empty

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures clear() on a full cache, then refilling it back to capacity.
static void ClearRefill_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        cache.clear();
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            cache.put(i, i);
    }
}
BENCHMARK(ClearRefill_LRUCache);

static void ClearRefill_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }

    for (auto _ : state) {
        cache.clear();
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            cache.put(i, i);
    }
}
BENCHMARK(ClearRefill_stdLRU);
