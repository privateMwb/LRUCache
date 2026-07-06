// LRUCache mixed read/write workload benchmark suite.
//
// Measures CachePro::LRUCache against a naive unordered_map + std::list
// implementation under realistic cache access patterns.
//
// Covers:
// - 80/20 get/put ratio with uniform key distribution
// - 95/5 get/put ratio with uniform key distribution
// - 80/20 get/put ratio with Zipfian (hot-key) access
// - 50/50 get/put ratio with a small cache (heavy eviction pressure)

#include <common/framework.h>
#include <reference/baseline.h>

#include <random>
#include <vector>

using namespace CachePro;


// Generates a Zipf-distributed sequence of keys in [0, n).
// Frequently accessed keys appear much more often than others,
// approximating real-world cache traffic.
static std::vector<int> generateZipfKeys(std::size_t count, std::size_t n, double s) {
    std::vector<double> weights(n);
    double sum = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        weights[i] = 1.0 / std::pow(static_cast<double>(i + 1), s);
        sum += weights[i];
    }

    std::mt19937 rng(42);
    std::discrete_distribution<int> dist(weights.begin(), weights.end());

    std::vector<int> keys(count);
    for (std::size_t i = 0; i < count; ++i) {
        keys[i] = dist(rng);
    }

    return keys;
}

// Generates a uniformly distributed sequence of keys in [0, n).
static std::vector<int> generateUniformKeys(std::size_t count, std::size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(n) - 1);

    std::vector<int> keys(count);
    for (std::size_t i = 0; i < count; ++i) {
        keys[i] = dist(rng);
    }

    return keys;
}


// Measures an 80% get / 20% put workload with uniformly distributed keys.
static void bench_workload_80_20_uniform() {
    constexpr std::size_t ops = 10'000;
    auto keys = generateUniformKeys(ops, 2000);

    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 10 < 8) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("CachePro 80/20 uniform", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1000);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 10 < 8) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("Naive LRU 80/20 uniform", 1000, nv);
}

// Measures a 95% get / 5% put workload with uniformly distributed keys.
static void bench_workload_95_5_uniform() {
    constexpr std::size_t ops = 10'000;
    auto keys = generateUniformKeys(ops, 2000);

    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 20 < 19) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("CachePro 95/5 uniform", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1000);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 20 < 19) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("Naive LRU 95/5 uniform", 1000, nv);
}

// Measures an 80% get / 20% put workload with Zipf-distributed keys.
// A small hot set receives most accesses, reflecting typical cache usage.
static void bench_workload_80_20_zipf() {
    constexpr std::size_t ops = 10'000;
    auto keys = generateZipfKeys(ops, 2000, 1.2);

    auto cp = [&] {
        LRUCache<int, int> c(1000);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 10 < 8) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("CachePro 80/20 zipf", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(1000);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 10 < 8) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("Naive LRU 80/20 zipf", 1000, nv);
}

// Measures a balanced 50% get / 50% put workload with a small cache,
// creating sustained eviction pressure.
static void bench_workload_50_50_small_capacity() {
    constexpr std::size_t ops = 10'000;
    auto keys = generateUniformKeys(ops, 5000);

    auto cp = [&] {
        LRUCache<int, int> c(200);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 2 == 0) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("CachePro 50/50 small cap", 1000, cp);

    auto nv = [&] {
        NaiveLRU<int, int> c(200);
        for (std::size_t i = 0; i < ops; ++i) {
            if (i % 2 == 0) doNotOptimize(c.get(keys[i]));
            else c.put(keys[i], keys[i]);
        }
        doNotOptimize(c);
    };
    BENCH("Naive LRU 50/50 small cap", 1000, nv);
}

// Executes all mixed workload benchmark cases.
static void run_benchmarks() {
    bench_workload_80_20_uniform();
    std::cout << "\n";

    bench_workload_95_5_uniform();
    std::cout << "\n";

    bench_workload_80_20_zipf();
    std::cout << "\n";

    bench_workload_50_50_small_capacity();
}

REGISTER_BENCH_SUITE();