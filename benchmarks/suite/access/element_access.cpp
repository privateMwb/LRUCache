// LRUCache Element Access Benchmark Suite
// Measures LRUCache get()/peek() performance against stdLRU.
//
// Each case builds its source cache once, outside the timed lambda —
// only repeated access into an already-full cache is measured.
//
// Covers:
// - get() on an existing key (recency-updating hit path)
// - peek() on an existing key (recency-neutral hit path)

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures get() on an existing key — updates recency order on every call.
static void bench_get_hit() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        int* v = cSrc.get(0);
        doNotOptimize(v);
    };

    auto s = [&] {
        int* v = sSrc.get(0);
        doNotOptimize(v);
    };

    BENCH("get() hit", c, s);
}

// Measures peek() on an existing key — does not affect recency order.
static void bench_peek_hit() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cSrc.put(i, i);
        sSrc.put(i, i);
    }

    auto c = [&] {
        const int* v = cSrc.peek(0);
        doNotOptimize(v);
    };

    auto s = [&] {
        const int* v = sSrc.peek(0);
        doNotOptimize(v);
    };

    BENCH("peek() hit", c, s);
}

// Executes all element access benchmark cases.
static void run_benchmarks() {
    bench_get_hit();
    std::cout << "\n";

    bench_peek_hit();
}

REGISTER_BENCH_SUITE();
