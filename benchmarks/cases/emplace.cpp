// LRUCache emplace benchmark suite.
// Compares CachePro::LRUCache's emplace() against put() with pre-constructed
// values, and against a naive std::unordered_map + std::list implementation.
//
// Covers:
// - emplace() versus put() with temporary values
// - emplace() on an existing key
// - emplace() with multi-argument construction
// - emplace() into a full cache

#include <common/bench_helper.h>
#include <common/bench_baseline.h>

#include <string>

using namespace CachePro;

// Measures insertion via emplace() versus put() with a temporary value.
static void bench_emplace_new_vs_put_temporary() {
    auto em = [&] {
        LRUCache<int, std::string> c(2000);
        for (int i = 0; i < 1000; ++i) c.emplace(i, "value");
        doNotOptimize(c);
    };
    BENCH("CachePro emplace new", SMALL, em);

    auto pt = [&] {
        LRUCache<int, std::string> c(2000);
        for (int i = 0; i < 1000; ++i) c.put(i, std::string("value"));
        doNotOptimize(c);
    };
    BENCH("CachePro put (temporary)", SMALL, pt);

    auto nv = [&] {
        NaiveLRU<int, std::string> c(2000);
        for (int i = 0; i < 1000; ++i) c.put(i, std::string("value"));
        doNotOptimize(c);
    };
    BENCH("Naive LRU put (temporary)", SMALL, nv);
}

// Measures updating an existing entry using emplace().
static void bench_emplace_existing_key() {
    LRUCache<int, std::string> c(1000);
    for (int i = 0; i < 1000; ++i) c.emplace(i, "value");

    auto em = [&] {
        c.emplace(500, "updated");
        doNotOptimize(c);
    };
    BENCH("CachePro emplace existing", LARGE, em);

    NaiveLRU<int, std::string> nc(1000);
    for (int i = 0; i < 1000; ++i) nc.put(i, "value");

    auto nv = [&] {
        nc.put(500, std::string("updated"));
        doNotOptimize(nc);
    };
    BENCH("Naive LRU put existing", LARGE, nv);
}

// Measures in-place construction using multiple constructor arguments.
static void bench_emplace_multi_arg() {
    struct Point {
        int x, y, z;
        Point(int a, int b, int c) : x(a), y(b), z(c) {}
    };

    auto em = [&] {
        LRUCache<int, Point> c(2000);
        for (int i = 0; i < 1000; ++i) c.emplace(i, i, i + 1, i + 2);
        doNotOptimize(c);
    };
    BENCH("CachePro emplace multi-arg", SMALL, em);

    auto pt = [&] {
        LRUCache<int, Point> c(2000);
        for (int i = 0; i < 1000; ++i) c.put(i, Point(i, i + 1, i + 2));
        doNotOptimize(c);
    };
    BENCH("CachePro put (constructed temp)", SMALL, pt);
}

// Measures emplace() while the cache remains at full capacity,
// forcing an eviction on every insertion.
static void bench_emplace_full_eviction() {
    LRUCache<int, std::string> c(1000);
    for (int i = 0; i < 1000; ++i) c.emplace(i, "value");

    int counter = 1000;
    auto em = [&] {
        c.emplace(counter++, "value");
        doNotOptimize(c);
    };
    BENCH("CachePro emplace (full, evicts)", LARGE, em);

    NaiveLRU<int, std::string> nc(1000);
    for (int i = 0; i < 1000; ++i) nc.put(i, "value");

    int nCounter = 1000;
    auto nv = [&] {
        nc.put(nCounter++, std::string("value"));
        doNotOptimize(nc);
    };
    BENCH("Naive LRU put (full, evicts)", LARGE, nv);
}

// Executes all emplace benchmark cases.
void run_emplace_benchmarks() {
    setHeader("Emplace Benchmarks");

    bench_emplace_new_vs_put_temporary();
    std::cout << "\n";

    bench_emplace_existing_key();
    std::cout << "\n";

    bench_emplace_multi_arg();
    std::cout << "\n";

    bench_emplace_full_eviction();
    borderLine();
    std::cout << "\n";
}