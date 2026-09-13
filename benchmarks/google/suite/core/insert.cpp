// LRUCache Insert Benchmark Suite
// Measures LRUCache put() performance against stdLRU on the pure-insert
// path — the cache never reaches capacity, so no eviction ever runs.
//
// Capacity is sized well above the iteration counts Google Benchmark
// is expected to run so the timed loop never crosses into the
// at-capacity/eviction path.
//
// Covers:
// - put() inserting a brand-new key, cache below capacity

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'200'000;
} // namespace

// Measures put() inserting a never-before-seen key, with room to spare.
static void PutInsert_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    int counter = 0;

    for (auto _ : state) {
        cache.put(counter, counter);
        ++counter;
    }
}
BENCHMARK(PutInsert_LRUCache);

static void PutInsert_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    int counter = 0;

    for (auto _ : state) {
        cache.put(counter, counter);
        ++counter;
    }
}
BENCHMARK(PutInsert_stdLRU);
