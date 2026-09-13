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

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 100;
} // namespace

// Measures move-assign, ping-ponging a populated cache between two slots.
static void bench_move_assign() {
    LRUCache<int, int> cA(kCapacity);
    LRUCache<int, int> cB(kCapacity);
    stdLRU<int, int> sA(kCapacity);
    stdLRU<int, int> sB(kCapacity);
    for (int i = 0; i < static_cast<int>(kCapacity); ++i) {
        cA.put(i, i);
        sA.put(i, i);
    }
    bool cFlip = false;
    bool sFlip = false;

    auto c = [&] {
        if (!cFlip)
            cB = std::move(cA);
        else
            cA = std::move(cB);
        cFlip = !cFlip;
    };

    auto s = [&] {
        if (!sFlip)
            sB = std::move(sA);
        else
            sA = std::move(sB);
        sFlip = !sFlip;
    };

    BENCH("move-assign", c, s);
}

// Measures move-construct from a freshly populated source (rebuild cost
// included — see file header note).
static void bench_move_construct() {
    auto c = [&] {
        LRUCache<int, int> src(kCapacity);
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            src.put(i, i);
        LRUCache<int, int> dst(std::move(src));
        doNotOptimize(dst);
    };

    auto s = [&] {
        stdLRU<int, int> src(kCapacity);
        for (int i = 0; i < static_cast<int>(kCapacity); ++i)
            src.put(i, i);
        stdLRU<int, int> dst(std::move(src));
        doNotOptimize(dst);
    };

    BENCH("move-construct", c, s);
}

// Executes all move benchmark cases.
static void run_benchmarks() {
    bench_move_assign();
    std::cout << "\n";

    bench_move_construct();
}

REGISTER_BENCH_SUITE();
