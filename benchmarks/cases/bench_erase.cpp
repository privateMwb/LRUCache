// LRUCache erase benchmark suite.
// Compares CachePro::LRUCache against a naive std::unordered_map +
// std::list implementation for common erase operations.
//
// Covers:
// - Erasing an existing key
// - Erasing a missing key
// - Erasing the most-recently-used (MRU) entry
// - Erasing the least-recently-used (LRU) entry
// - Repeated erase -> reinsert cycles

#include <common/bench_helper.h>
#include <common/bench_baseline.h>

using namespace CachePro;

// Measures the cost of erasing an existing key from the cache.
static void bench_erase_existing() {
    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        bool ok = c.erase(500);
        doNotOptimize(ok);
    };
    BENCH("CachePro erase existing", SMALL, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        bool ok = c.erase(500);
        doNotOptimize(ok);
    };
    BENCH("Naive LRU erase existing", SMALL, nv);
}

// Measures the cost of attempting to erase a key that does not exist.
static void bench_erase_missing() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i) c.put(i, i);

    auto cp = [&] {
        bool ok = c.erase(-1);
        doNotOptimize(ok);
    };
    BENCH("CachePro erase missing", LARGE, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i) nc.put(i, i);

    auto nv = [&] {
        bool ok = nc.erase(-1);
        doNotOptimize(ok);
    };
    BENCH("Naive LRU erase missing", LARGE, nv);
}

// Measures the cost of erasing the current most-recently-used entry.
static void bench_erase_mru() {
    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        bool ok = c.erase(999);   // Last inserted entry is the MRU.
        doNotOptimize(ok);
    };
    BENCH("CachePro erase MRU", SMALL, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        bool ok = c.erase(999);
        doNotOptimize(ok);
    };
    BENCH("Naive LRU erase MRU", SMALL, nv);
}

// Measures the cost of erasing the current least-recently-used entry.
static void bench_erase_lru() {
    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        bool ok = c.erase(0);   // First inserted entry is the LRU.
        doNotOptimize(ok);
    };
    BENCH("CachePro erase LRU", SMALL, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        bool ok = c.erase(0);
        doNotOptimize(ok);
    };
    BENCH("Naive LRU erase LRU", SMALL, nv);
}

// Measures repeated erase -> reinsert cycles.
// Highlights free-list reuse versus repeated heap allocation.
static void bench_erase_reinsert_cycle() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i) c.put(i, i);

    auto cp = [&] {
        (void)c.erase(500);
        c.put(500, 500);
        doNotOptimize(c);
    };
    BENCH("CachePro erase/reinsert cycle", LARGE, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i) nc.put(i, i);

    auto nv = [&] {
        nc.erase(500);
        nc.put(500, 500);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU erase/reinsert cycle", LARGE, nv);
}

// Executes all erase benchmark cases.
void run_erase_benchmarks() {
    setHeader("Erase Benchmarks");

    bench_erase_existing();
    std::cout << "\n";

    bench_erase_missing();
    std::cout << "\n";

    bench_erase_mru();
    std::cout << "\n";

    bench_erase_lru();
    std::cout << "\n";

    bench_erase_reinsert_cycle();
    borderLine();
    std::cout << "\n";
}