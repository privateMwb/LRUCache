// LRUCache Clear-and-Refill Benchmark Suite
// Measures LRUCache clear() + refill performance against stdLRU.
//
// Each timed call clears a full cache back to empty, then refills it to
// capacity again — a full teardown/rebuild cycle. Capacity is kept
// small since each call does O(capacity) work, not O(1).
//
// Covers:
// - clear() on a full cache, then refill from empty

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures clear() on a full cache, then refilling it back to capacity.
static void bench_clear_refill() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        cSrc.clear();
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            cSrc.put(i, i);
    };

    auto s = [&] {
        sSrc.clear();
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            sSrc.put(i, i);
    };

    BENCH("clear() + refill", c, s);
}

// Executes all clear-and-refill benchmark cases.
static void run_benchmarks() {
    bench_clear_refill();
}

REGISTER_BENCH_SUITE();
