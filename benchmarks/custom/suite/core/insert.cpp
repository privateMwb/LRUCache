// LRUCache Insert Benchmark Suite
// Measures LRUCache put() performance against stdLRU on the pure-insert
// path — the cache never reaches capacity, so no eviction ever runs.
//
// Capacity is sized above the combined 10K + 100K + 1M call count so a
// single BENCH() run (which reuses the same cache across all three
// tiers) never crosses into the at-capacity/eviction path.
//
// Covers:
// - put() inserting a brand-new key, cache below capacity

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
// 10K + 100K + 1M = 1,110,000 total put() calls across one BENCH() run.
constexpr std::size_t kCapacity = 1'200'000;
} // namespace

// Measures put() inserting a never-before-seen key, with room to spare.
static void bench_put_insert() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    int cCounter = 0;
    int sCounter = 0;

    auto c = [&] {
        cSrc.put(cCounter, cCounter);
        ++cCounter;
    };

    auto s = [&] {
        sSrc.put(sCounter, sCounter);
        ++sCounter;
    };

    BENCH("put() insert", c, s);
}

// Executes all insert benchmark cases.
static void run_benchmarks() {
    bench_put_insert();
}

REGISTER_BENCH_SUITE();
