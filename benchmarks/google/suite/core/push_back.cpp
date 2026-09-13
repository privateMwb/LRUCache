// LRUCache Steady-State Insert Benchmark Suite
// Measures LRUCache put() performance against stdLRU on the at-capacity
// path — the cache starts full, so every call evicts the least recently
// used entry before inserting the new one.
//
// This is where LRUCache's pool-and-free-list reuse should separate
// from stdLRU's per-node heap alloc/dealloc on every eviction.
//
// Covers:
// - put() inserting a brand-new key, cache already at capacity

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures put() at capacity — every call evicts the LRU entry first.
static void PutInsertEvicting_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }
    int counter = static_cast<int>(kCapacity);

    for (auto _ : state) {
        cache.put(counter, counter);
        ++counter;
    }
}
BENCHMARK(PutInsertEvicting_LRUCache);

static void PutInsertEvicting_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cache.put(i, i);
    }
    int counter = static_cast<int>(kCapacity);

    for (auto _ : state) {
        cache.put(counter, counter);
        ++counter;
    }
}
BENCHMARK(PutInsertEvicting_stdLRU);
