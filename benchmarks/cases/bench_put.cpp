// LRUCache put benchmarks.
//
// Compares CachePro::LRUCache against the naive unordered_map + std::list
// implementation for insertion, update, move, and eviction scenarios.
//
// Covers:
// - put() with copied keys and values
// - put() with moved values
// - put() updating an existing key
// - put() at full capacity

#include <common/bench_helper.h>
#include <common/bench_baseline.h>

#include <string>

using namespace CachePro;

// Measures put() using copied keys and values.
static void bench_put_copy() {
    auto cp = [&] {
        LRUCache<int, int> c(2000);
        for (int i = 0; i < 1000; ++i)
            c.put(i, i);
        doNotOptimize(c);
    };
    BENCH("CachePro put copy", SMALL, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(2000);
        for (int i = 0; i < 1000; ++i)
            c.put(i, i);
        doNotOptimize(c);
    };
    BENCH("Naive LRU put copy", SMALL, nv);
}

// Measures put() using moved values.
static void bench_put_move() {
    auto cp = [&] {
        LRUCache<int, std::string> c(2000);
        for (int i = 0; i < 1000; ++i)
            c.put(i, std::string("value") + std::to_string(i));
        doNotOptimize(c);
    };
    BENCH("CachePro put move", SMALL, cp);

    auto nv = [&] {
        NaiveLRU<int, std::string> c(2000);
        for (int i = 0; i < 1000; ++i)
            c.put(i, std::string("value") + std::to_string(i));
        doNotOptimize(c);
    };
    BENCH("Naive LRU put move", SMALL, nv);
}

// Measures put() when updating an existing key.
static void bench_put_update_existing() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    auto cp = [&] {
        c.put(500, 999);
        doNotOptimize(c);
    };
    BENCH("CachePro put update", SMALL, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i)
        nc.put(i, i);

    auto nv = [&] {
        nc.put(500, 999);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU put update", SMALL, nv);
}

// Measures put() at full capacity, forcing an eviction on every insertion.
static void bench_put_full_eviction() {
    LRUCache<int, int> c(1000);
    for (int i = 0; i < 1000; ++i)
        c.put(i, i);

    int counter = 0;
    auto cp = [&] {
        c.put(counter++, counter);
        doNotOptimize(c);
    };
    BENCH("CachePro put (full, evicts)", SMALL, cp);

    NaiveLRU<int, int> nc(1000);
    for (int i = 0; i < 1000; ++i)
        nc.put(i, i);

    int nCounter = 0;
    auto nv = [&] {
        nc.put(nCounter++, nCounter);
        doNotOptimize(nc);
    };
    BENCH("Naive LRU put (full, evicts)", SMALL, nv);
}

// Runs all put benchmarks.
void run_put_benchmarks() {
    setHeader("Put Benchmarks");

    bench_put_copy();
    std::cout << "\n";

    bench_put_move();
    std::cout << "\n";

    bench_put_update_existing();
    std::cout << "\n";

    bench_put_full_eviction();
    borderLine();
    std::cout << "\n";
}