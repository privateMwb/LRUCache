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
// Capacity is sized above the combined 10K + 100K + 1M call count so a
// single BENCH() run never crosses into the at-capacity/eviction path.
//
// Covers:
// - emplace() constructing a brand-new value in place, cache below capacity

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
// 10K + 100K + 1M = 1,110,000 total emplace() calls across one BENCH() run.
constexpr std::size_t kCapacity = 1'200'000;
} // namespace

// Measures emplace() constructing a never-before-seen value in place.
static void bench_emplace_insert() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    int cCounter = 0;
    int sCounter = 0;

    auto c = [&] {
        int& v = cSrc.emplace(cCounter, cCounter);
        doNotOptimize(v);
        ++cCounter;
    };

    auto s = [&] {
        int& v = sSrc.emplace(sCounter, sCounter);
        doNotOptimize(v);
        ++sCounter;
    };

    BENCH("emplace() insert", c, s);
}

// Executes all emplace benchmark cases.
static void run_benchmarks() {
    bench_emplace_insert();
}

REGISTER_BENCH_SUITE();
