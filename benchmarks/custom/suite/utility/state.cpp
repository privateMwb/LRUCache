// LRUCache Size/Empty/Capacity Benchmark Suite
// Measures LRUCache's size(), empty(), and capacity() against stdLRU's
// direct equivalents.
//
// Covers:
// - size() (paired against stdLRU)
// - empty() (paired against stdLRU)
// - capacity() (paired against stdLRU)

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
} // namespace

// Measures size() against stdLRU's size().
static void bench_size() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        std::size_t v = cSrc.size();
        doNotOptimize(v);
    };

    auto s = [&] {
        std::size_t v = sSrc.size();
        doNotOptimize(v);
    };

    BENCH("size()", c, s);
}

// Measures empty() against stdLRU's empty().
static void bench_empty() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        bool v = cSrc.empty();
        doNotOptimize(v);
    };

    auto s = [&] {
        bool v = sSrc.empty();
        doNotOptimize(v);
    };

    BENCH("empty()", c, s);
}

// Measures capacity() against stdLRU's capacity().
static void bench_capacity() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);

    auto c = [&] {
        std::size_t v = cSrc.capacity();
        doNotOptimize(v);
    };

    auto s = [&] {
        std::size_t v = sSrc.capacity();
        doNotOptimize(v);
    };

    BENCH("capacity()", c, s);
}

// Executes all size/empty/capacity benchmark cases.
static void run_benchmarks() {
    bench_size();
    std::cout << "\n";

    bench_empty();
    std::cout << "\n";

    bench_capacity();
}

REGISTER_BENCH_SUITE();
