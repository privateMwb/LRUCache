// LRUCache Erase Benchmark Suite
// Measures LRUCache erase() performance against stdLRU.
//
// Both caches are pre-filled with one unique key per call the BENCH
// macro will make across all three tiers combined (10K + 100K + 1M),
// so every timed erase() removes a real, still-present entry rather
// than hitting the not-found path partway through the run.
//
// Covers:
// - erase() on an existing key

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
// 10K + 100K + 1M = 1,110,000 total erase() calls across one BENCH() run.
constexpr std::size_t kCapacity = 1'200'000;
} // namespace

// Measures erase() removing an existing key.
static void bench_erase_existing() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }
    int cCounter = 0;
    int sCounter = 0;

    auto c = [&] {
        bool v = cSrc.erase(cCounter);
        doNotOptimize(v);
        ++cCounter;
    };

    auto s = [&] {
        bool v = sSrc.erase(sCounter);
        doNotOptimize(v);
        ++sCounter;
    };

    BENCH("erase() existing", c, s);
}

// Executes all erase benchmark cases.
static void run_benchmarks() {
    bench_erase_existing();
}

REGISTER_BENCH_SUITE();
