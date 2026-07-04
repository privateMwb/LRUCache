// LRUCache resize benchmark suite.
//
// Measures CachePro::LRUCache resize() performance. Since the naive baseline
// has no resize() operation, it is compared against the equivalent manual
// approach of constructing a new cache and reinserting the surviving entries.
//
// Covers:
// - Growing the cache capacity
// - Shrinking with eviction
// - Shrinking without eviction
// - Repeated grow/shrink cycles

#include <common/bench_helper.h>
#include <common/bench_baseline.h>

using namespace CachePro;

// Measures growing capacity, which rebuilds the node pool and preserves entries.
static void bench_resize_grow() {
    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (int i = 0; i < 1000; ++i) c.put(i, i);
        c.resize(2000);
        doNotOptimize(c);
    };
    BENCH("CachePro resize grow 1K->2K", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> old(1000);
        for (int i = 0; i < 1000; ++i) old.put(i, i);

        NaiveLRU<int, int> grown(2000);
        for (int i = 0; i < 1000; ++i) grown.put(i, i);
        doNotOptimize(grown);
    };
    BENCH("Naive LRU manual grow 1K->2K", 1000, nv);
}

// Measures shrinking capacity, which evicts entries before rebuilding the pool.
static void bench_resize_shrink_evicts() {
    auto cp = [&] {
        LRUCache<int, int> c(2000);
        for (int i = 0; i < 2000; ++i) c.put(i, i);
        c.resize(500);
        doNotOptimize(c);
    };
    BENCH("CachePro resize shrink 2K->500", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> old(2000);
        for (int i = 0; i < 2000; ++i) old.put(i, i);

        NaiveLRU<int, int> shrunk(500);
        for (int i = 1500; i < 2000; ++i) shrunk.put(i, i);
        doNotOptimize(shrunk);
    };
    BENCH("Naive LRU manual shrink 2K->500", 1000, nv);
}

// Measures shrinking to the current size without requiring any eviction.
static void bench_resize_shrink_to_current() {
    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (int i = 0; i < 500; ++i) c.put(i, i);
        c.resize(500);
        doNotOptimize(c);
    };
    BENCH("CachePro resize no-op shrink", 1000, cp);
}

// Measures repeated grow and shrink operations on the same cache instance.
static void bench_resize_grow_shrink_cycle() {
    auto cp = [&] {
        LRUCache<int, int> c(500);
        for (int i = 0; i < 500; ++i) c.put(i, i);
        c.resize(1000);
        c.resize(250);
        c.resize(800);
        doNotOptimize(c);
    };
    BENCH("CachePro resize grow/shrink cycle", 1000, cp);
}

// Executes all resize benchmarks.
void run_resize_benchmarks() {
    setHeader("Resize Benchmarks");

    bench_resize_grow();
    std::cout << "\n";

    bench_resize_shrink_evicts();
    std::cout << "\n";

    bench_resize_shrink_to_current();
    std::cout << "\n";

    bench_resize_grow_shrink_cycle();
    borderLine();
    std::cout << "\n";
}