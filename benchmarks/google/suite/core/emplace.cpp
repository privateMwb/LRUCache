// LRUCache Emplace Benchmark Suite
// Measures LRUCache emplace() performance against stdLRU's emplace(),
// on the pure-insert path — the cache never reaches capacity, so no
// eviction ever runs.
//
// stdLRU has no native in-place-keyed-insert primitive (std::list and
// std::unordered_map don't offer one together), so its emplace() is a
// rough equivalent added purely for this comparison: it still
// constructs V(args...) directly, but through the same
// find-then-splice-or-insert path put() uses. LRUCache's emplace()
// forwards straight into the value's constructor with no intermediate
// V temporary — that's the gap this benchmark is meant to surface.
//
// Capacity is sized well above the iteration counts Google Benchmark
// is expected to run so the timed loop never crosses into the
// at-capacity/eviction path.
//
// Covers:
// - emplace() constructing a brand-new value in place, cache below capacity

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'200'000;
} // namespace

// Measures emplace() constructing a never-before-seen value in place.
static void EmplaceInsert_LRUCache(benchmark::State& state) {
    LRUCache<int, int> cache(kCapacity);
    int counter = 0;

    for (auto _ : state) {
        int& v = cache.emplace(counter, counter);
        benchmark::DoNotOptimize(v);
        ++counter;
    }
}
BENCHMARK(EmplaceInsert_LRUCache);

static void EmplaceInsert_stdLRU(benchmark::State& state) {
    stdLRU<int, int> cache(kCapacity);
    int counter = 0;

    for (auto _ : state) {
        int& v = cache.emplace(counter, counter);
        benchmark::DoNotOptimize(v);
        ++counter;
    }
}
BENCHMARK(EmplaceInsert_stdLRU);
