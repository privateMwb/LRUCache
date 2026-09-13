// LRUCache Move Benchmark Suite
// Measures LRUCache move-construct and move-assign performance against
// stdLRU.
//
// Move-assign ping-pongs a single populated cache back and forth
// between two slots (a -> b, then b -> a, ...), so no per-call rebuild
// cost leaks into the measurement — only the move itself is timed.
//
// Move-construct can't avoid that rebuild cost as cleanly: a fresh
// source has to exist before every constructing move, since a
// moved-from cache is left empty. Each timed call rebuilds the source
// first, so this number reflects "populate + move-construct", not the
// move alone — noted rather than hidden.
//
// Covers:
// - move-assign between two populated caches
// - move-construct from a freshly populated source

#include <CachePro/LRUCache.h>
#include <benchmark/benchmark.h>
#include <support/reference.h>

#include <cstddef>
#include <utility>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures move-assign, ping-ponging a populated cache between two slots.
static void MoveAssign_LRUCache(benchmark::State& state) {
    LRUCache<int, int> a(kCapacity);
    LRUCache<int, int> b(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        a.put(i, i);
    }
    bool flip = false;

    for (auto _ : state) {
        if (!flip)
            b = std::move(a);
        else
            a = std::move(b);
        flip = !flip;
    }
}
BENCHMARK(MoveAssign_LRUCache);

static void MoveAssign_stdLRU(benchmark::State& state) {
    stdLRU<int, int> a(kCapacity);
    stdLRU<int, int> b(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        a.put(i, i);
    }
    bool flip = false;

    for (auto _ : state) {
        if (!flip)
            b = std::move(a);
        else
            a = std::move(b);
        flip = !flip;
    }
}
BENCHMARK(MoveAssign_stdLRU);

// Measures move-construct from a freshly populated source (rebuild cost
// included — see file header note).
static void MoveConstruct_LRUCache(benchmark::State& state) {
    for (auto _ : state) {
        LRUCache<int, int> src(kCapacity);
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            src.put(i, i);
        LRUCache<int, int> dst(std::move(src));
        benchmark::DoNotOptimize(dst);
    }
}
BENCHMARK(MoveConstruct_LRUCache);

static void MoveConstruct_stdLRU(benchmark::State& state) {
    for (auto _ : state) {
        stdLRU<int, int> src(kCapacity);
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            src.put(i, i);
        stdLRU<int, int> dst(std::move(src));
        benchmark::DoNotOptimize(dst);
    }
}
BENCHMARK(MoveConstruct_stdLRU);
