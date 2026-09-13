// LRUCache Reallocation Benchmark Suite
// Measures LRUCache resize() growing performance against stdLRU.
//
// Each timed call grows capacity by one entry over the last, so every
// call does real pool + table rebuild work (LRUCache) or an implicit
// rehash pass (stdLRU) rather than hitting a no-op "already big enough"
// path. The cache is seeded with a small live population so the
// per-call rebuild walks real, still-linked entries.
//
// Covers:
// - resize() growing capacity by one entry, repeated

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kSeedCount = 100;
constexpr std::size_t kInitialCapacity = 100;
} // namespace

// Measures resize() growing capacity by one entry on every call.
static void bench_resize_grow() {
    LRUCache<int, int> cSrc(kInitialCapacity);
    stdLRU<int, int> sSrc(kInitialCapacity);
    for (int i = 0; i < static_cast<int>(kSeedCount); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }
    std::size_t cGrowth = 0;
    std::size_t sGrowth = 0;

    auto c = [&] { cSrc.resize(kInitialCapacity + ++cGrowth); };

    auto s = [&] { sSrc.resize(kInitialCapacity + ++sGrowth); };

    BENCH("resize() grow", c, s);
}

// Executes all reallocation benchmark cases.
static void run_benchmarks() {
    bench_resize_grow();
}

REGISTER_BENCH_SUITE();
