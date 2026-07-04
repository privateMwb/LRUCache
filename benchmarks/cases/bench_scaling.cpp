// LRUCache scaling benchmark suite.
//
// Measures how CachePro::LRUCache scales as cache capacity increases,
// highlighting the effects of cache locality and larger hash tables.
//
// Covers:
// - Filling an empty cache at increasing capacities
// - Steady-state get() performance
// - Steady-state put() performance with continuous eviction

#include <common/bench_helper.h>
#include <common/bench_baseline.h>

using namespace CachePro;

// Measures the cost of filling an empty cache at increasing capacities.
static void bench_scaling_fill() {
    setSubHeader("Fill from empty");

    auto cp1k = [&] { LRUCache<int, int> c(1'000);   for (int i = 0; i < 1'000;   ++i) c.put(i, i); doNotOptimize(c); };
    BENCH("CachePro fill 1K", 1000, cp1k);
    auto nv1k = [&] { NaiveLRU<int, int> c(1'000);   for (int i = 0; i < 1'000;   ++i) c.put(i, i); doNotOptimize(c); };
    BENCH("Naive LRU fill 1K", 1000, nv1k);

    auto cp10k = [&] { LRUCache<int, int> c(10'000); for (int i = 0; i < 10'000; ++i) c.put(i, i); doNotOptimize(c); };
    BENCH("CachePro fill 10K", 100, cp10k);
    auto nv10k = [&] { NaiveLRU<int, int> c(10'000); for (int i = 0; i < 10'000; ++i) c.put(i, i); doNotOptimize(c); };
    BENCH("Naive LRU fill 10K", 100, nv10k);

    auto cp100k = [&] { LRUCache<int, int> c(100'000); for (int i = 0; i < 100'000; ++i) c.put(i, i); doNotOptimize(c); };
    BENCH("CachePro fill 100K", 10, cp100k);
    auto nv100k = [&] { NaiveLRU<int, int> c(100'000); for (int i = 0; i < 100'000; ++i) c.put(i, i); doNotOptimize(c); };
    BENCH("Naive LRU fill 100K", 10, nv100k);

    static LRUCache<int, int> cpMillion(1'000'000);
    auto cp1m = [&] { for (int i = 0; i < 1'000'000; ++i) cpMillion.put(i, i); doNotOptimize(cpMillion); };
    BENCH("CachePro fill 1M", 1, cp1m);

    static NaiveLRU<int, int> nvMillion(1'000'000);
    auto nv1m = [&] { for (int i = 0; i < 1'000'000; ++i) nvMillion.put(i, i); doNotOptimize(nvMillion); };
    BENCH("Naive LRU fill 1M", 1, nv1m);
}

// Measures steady-state get() throughput after the cache has been filled.
static void bench_scaling_get() {
    setSubHeader("Steady-state get");

    LRUCache<int, int> c1k(1'000);
    for (int i = 0; i < 1'000; ++i) c1k.put(i, i);
    auto cp1k = [&] { doNotOptimize(c1k.get(500)); };
    BENCH("CachePro get @1K", 1000, cp1k);

    LRUCache<int, int> c100k(100'000);
    for (int i = 0; i < 100'000; ++i) c100k.put(i, i);
    auto cp100k = [&] { doNotOptimize(c100k.get(50'000)); };
    BENCH("CachePro get @100K", 10, cp100k);

    LRUCache<int, int> c1m(1'000'000);
    for (int i = 0; i < 1'000'000; ++i) c1m.put(i, i);
    auto cp1m = [&] { doNotOptimize(c1m.get(500'000)); };
    BENCH("CachePro get @1M", 1, cp1m);

    NaiveLRU<int, int> n1k(1'000);
    for (int i = 0; i < 1'000; ++i) n1k.put(i, i);
    auto nv1k = [&] { doNotOptimize(n1k.get(500)); };
    BENCH("Naive LRU get @1K", 1000, nv1k);

    NaiveLRU<int, int> n100k(100'000);
    for (int i = 0; i < 100'000; ++i) n100k.put(i, i);
    auto nv100k = [&] { doNotOptimize(n100k.get(50'000)); };
    BENCH("Naive LRU get @100K", 10, nv100k);

    NaiveLRU<int, int> n1m(1'000'000);
    for (int i = 0; i < 1'000'000; ++i) n1m.put(i, i);
    auto nv1m = [&] { doNotOptimize(n1m.get(500'000)); };
    BENCH("Naive LRU get @1M", 1, nv1m);
}

// Measures steady-state put() throughput while continuously evicting entries.
static void bench_scaling_evict() {
    setSubHeader("Steady-state evict");

    LRUCache<int, int> c1k(1'000);
    for (int i = 0; i < 1'000; ++i) c1k.put(i, i);
    int ctr1k = 1'000;
    auto cp1k = [&] { c1k.put(ctr1k++, ctr1k); };
    BENCH("CachePro evict @1K", 1000, cp1k);

    LRUCache<int, int> c100k(100'000);
    for (int i = 0; i < 100'000; ++i) c100k.put(i, i);
    int ctr100k = 100'000;
    auto cp100k = [&] { c100k.put(ctr100k++, ctr100k); };
    BENCH("CachePro evict @100K", 10, cp100k);

    LRUCache<int, int> c1m(1'000'000);
    for (int i = 0; i < 1'000'000; ++i) c1m.put(i, i);
    int ctr1m = 1'000'000;
    auto cp1m = [&] { c1m.put(ctr1m++, ctr1m); };
    BENCH("CachePro evict @1M", 1, cp1m);

    NaiveLRU<int, int> n1k(1'000);
    for (int i = 0; i < 1'000; ++i) n1k.put(i, i);
    int nctr1k = 1'000;
    auto nv1k = [&] { n1k.put(nctr1k++, nctr1k); };
    BENCH("Naive LRU evict @1K", 1000, nv1k);

    NaiveLRU<int, int> n100k(100'000);
    for (int i = 0; i < 100'000; ++i) n100k.put(i, i);
    int nctr100k = 100'000;
    auto nv100k = [&] { n100k.put(nctr100k++, nctr100k); };
    BENCH("Naive LRU evict @100K", 10, nv100k);

    NaiveLRU<int, int> n1m(1'000'000);
    for (int i = 0; i < 1'000'000; ++i) n1m.put(i, i);
    int nctr1m = 1'000'000;
    auto nv1m = [&] { n1m.put(nctr1m++, nctr1m); };
    BENCH("Naive LRU evict @1M", 1, nv1m);
}

// Executes all scaling benchmarks.
void run_scaling_benchmarks() {
    setHeader("Scaling Benchmarks");

    bench_scaling_fill();
    std::cout << "\n";

    bench_scaling_get();
    std::cout << "\n";

    bench_scaling_evict();
    borderLine();
    std::cout << "\n";
}