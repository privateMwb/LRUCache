// LRUCache eviction benchmark suite.
// Compares CachePro::LRUCache's fixed-pool eviction against a naive
// std::unordered_map + std::list implementation that performs heap
// allocation/deallocation during eviction.
//
// Covers:
// - Steady-state eviction at full capacity (small and large caches)
// - Single eviction cost
// - Repeated fill -> clear -> refill cycles

#include <common/framework.h>
#include <reference/baseline.h>

using namespace CachePro;

// Measures steady-state eviction throughput with a small cache.
// Every put() inserts a new key and evicts the current least-recently-used entry.
static void bench_eviction_steady_state_small() {
    LRUCache<int, int> c(100);
    for (int i = 0; i < 100; ++i) c.put(i, i);

    int counter = 100;
    auto cp = [&] {
        c.put(counter++, counter);
        doNotOptimize(c);
    };
    BENCH("CachePro evict steady (cap 100)", LARGE, cp);

    NaiveLRU<int, int> nc(100);
    for (int i = 0; i < 100; ++i) nc.put(i, i);

    int nCounter = 100;
    auto nv = [&] {
        nc.put(nCounter++, nCounter);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU evict steady (cap 100)", LARGE, nv);
}

// Measures steady-state eviction throughput with a large cache.
// Highlights scalability as cache size grows while remaining at full capacity.
static void bench_eviction_steady_state_large() {
    LRUCache<int, int> c(100'000);
    for (int i = 0; i < 100'000; ++i) c.put(i, i);

    int counter = 100'000;
    auto cp = [&] {
        c.put(counter++, counter);
        doNotOptimize(c);
    };
    BENCH("CachePro evict steady (cap 100K)", LARGE, cp);

    NaiveLRU<int, int> nc(100'000);
    for (int i = 0; i < 100'000; ++i) nc.put(i, i);

    int nCounter = 100'000;
    auto nv = [&] {
        nc.put(nCounter++, nCounter);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU evict steady (cap 100K)", LARGE, nv);
}

// Measures the cost of a single eviction triggered by one insertion
// into a full cache.
static void bench_single_eviction() {
    auto cp = [&] {
        LRUCache<int, int> c(1);
        c.put(1, 1);
        c.put(2, 2);   // Triggers exactly one eviction.
        doNotOptimize(c);
    };
    BENCH("CachePro single eviction", MEDIUM, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1);
        c.put(1, 1);
        c.put(2, 2);
        doNotOptimize(c);
    };
    BENCH("Naive LRU single eviction", MEDIUM, nv);
}

// Measures repeated fill -> clear -> refill cycles.
// Highlights free-list reuse versus repeated heap allocation across
// complete cache lifecycle resets.
static void bench_fill_clear_cycle() {
    auto cp = [&] {
        LRUCache<int, int> c(500);
        for (int i = 0; i < 500; ++i) c.put(i, i);
        c.clear();
        for (int i = 0; i < 500; ++i) c.put(i, i);
        doNotOptimize(c);
    };
    BENCH("CachePro fill/clear/refill", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(500);
        for (int i = 0; i < 500; ++i) c.put(i, i);
        c.clear();
        for (int i = 0; i < 500; ++i) c.put(i, i);
        doNotOptimize(c);
    };
    BENCH("Naive LRU fill/clear/refill", 1000, nv);
}

// Executes all eviction benchmark cases.
static void run_benchmarks() {
    bench_eviction_steady_state_small();
    std::cout << "\n";

    bench_eviction_steady_state_large();
    std::cout << "\n";

    bench_single_eviction();
    std::cout << "\n";

    bench_fill_clear_cycle();
}

REGISTER_BENCH_SUITE();