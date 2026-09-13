// LRUCache Shrink-to-Fit Benchmark Suite
// Measures LRUCache shrink_to_fit() performance against stdLRU.
//
// Each call adds a bounded, cycling amount of slack then reclaims it,
// so per-call cost stays bounded across the whole run.

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kSeedCount = 100;
constexpr std::size_t kMaxGrowth = 64;
} // namespace

static void bench_shrink_to_fit() {
    LRUCache<int, int> cSrc(kSeedCount);
    stdLRU<int, int> sSrc(kSeedCount);
    for (int i = 0; i < static_cast<int>(kSeedCount); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    std::size_t cGrowth = 1;
    std::size_t sGrowth = 1;

    auto c = [&] {
        std::size_t growth = (cGrowth++ % kMaxGrowth) + 1;
        cSrc.resize(kSeedCount + growth); // add slack
        cSrc.shrink_to_fit();             // reclaim it
    };

    auto s = [&] {
        std::size_t growth = (sGrowth++ % kMaxGrowth) + 1;
        sSrc.resize(kSeedCount + growth);
        sSrc.resize(kSeedCount);
    };

    BENCH("shrink_to_fit()", c, s);
}

static void run_benchmarks() {
    bench_shrink_to_fit();
}

REGISTER_BENCH_SUITE();
