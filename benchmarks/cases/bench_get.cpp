// LRUCache get-related benchmarks.
//
// Compares CachePro::LRUCache against the naive unordered_map + std::list
// implementation for lookup operations and LRU maintenance overhead.
//
// Covers:
// - Cache hit requiring MRU relinking
// - Cache hit on the current MRU entry
// - Cache miss
// - peek() vs get()
// - contains()

#include <common/bench_helper.h>
#include <common/bench_baseline.h>

using namespace CachePro;

// Measures get() on a cache hit that promotes the accessed entry
// from LRU to MRU by relinking it in the usage list.
static void bench_get_hit_relink() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    auto cp = [&] {
        (void)c.get(0);
        doNotOptimize(c);
    };
    BENCH("CachePro get hit (relink)", LARGE, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i)
        nc.put(i, i);

    auto nv = [&] {
        nc.get(0);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU get hit (relink)", LARGE, nv);
}

// Measures get() when the requested entry is already the
// most recently used and requires no relinking.
static void bench_get_hit_already_mru() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    (void)c.get(999);

    auto cp = [&] {
        (void)c.get(999);
        doNotOptimize(c);
    };
    BENCH("CachePro get hit (already MRU)", LARGE, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i)
        nc.put(i, i);

    nc.get(999);

    auto nv = [&] {
        nc.get(999);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU get hit (already MRU)", LARGE, nv);
}

// Measures get() on a missing key.
static void bench_get_miss() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    auto cp = [&] {
        auto* v = c.get(-1);
        doNotOptimize(v);
    };
    BENCH("CachePro get miss", LARGE, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i)
        nc.put(i, i);

    auto nv = [&] {
        auto* v = nc.get(-1);
        doNotOptimize(v);
    };
    BENCH("Naive LRU get miss", LARGE, nv);
}

// Compares peek() against get() to isolate the cost of
// maintaining LRU ordering during lookups.
static void bench_peek_vs_get() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    auto pk = [&] {
        auto* v = c.peek(500);
        doNotOptimize(v);
    };
    BENCH("CachePro peek (no reorder)", LARGE, pk);

    auto gt = [&] {
        auto* v = c.get(500);
        doNotOptimize(v);
    };
    BENCH("CachePro get (reorder)", LARGE, gt);
}

// Measures contains() lookup performance.
static void bench_contains() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    auto cp = [&] {
        bool found = c.contains(500);
        doNotOptimize(found);
    };
    BENCH("CachePro contains", LARGE, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i)
        nc.put(i, i);

    auto nv = [&] {
        bool found = nc.contains(500);
        doNotOptimize(found);
    };
    BENCH("Naive LRU contains", LARGE, nv);
}

// Runs all get benchmarks.
void run_get_benchmarks() {
    setHeader("Get Benchmarks");

    bench_get_hit_relink();
    std::cout << "\n";

    bench_get_hit_already_mru();
    std::cout << "\n";

    bench_get_miss();
    std::cout << "\n";

    bench_peek_vs_get();
    std::cout << "\n";

    bench_contains();
    borderLine();
    std::cout << "\n";
}