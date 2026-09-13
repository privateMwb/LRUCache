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

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures put() at capacity — every call evicts the LRU entry first.
static void bench_put_insert_evicting() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }
    int cCounter = static_cast<int>(kCapacity);
    int sCounter = static_cast<int>(kCapacity);

    auto c = [&] {
        cSrc.put(cCounter, cCounter);
        ++cCounter;
    };

    auto s = [&] {
        sSrc.put(sCounter, sCounter);
        ++sCounter;
    };

    BENCH("put() insert (evicting)", c, s);
}

// Executes all steady-state insert benchmark cases.
static void run_benchmarks() {
    bench_put_insert_evicting();
}

REGISTER_BENCH_SUITE();
