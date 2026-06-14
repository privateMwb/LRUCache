// LRUCache Benchmark Suite
// Measures core cache operations:
//
// - put without reserve     (insert with internal bucket allocation)
// - put with reserve        (insert with pre-allocated buckets)
// - get hit                 (lookup with LRU promotion, key always present)
// - get miss                (lookup cost when key is absent)
// - peek                    (lookup without LRU promotion)
// - erase                   (removal by key)
// - eviction                (insert at capacity, forcing LRU eviction)
// - contains                (key existence check)
// - mixed workload          (interleaved put, get, and eviction)
//
// Benchmarks use std::unordered_map as a reference baseline.

#include <iostream>
#include <chrono>
#include <cstddef>
#include <unordered_map>
#include <string>

#include "LRUCache.h"
#include "utils/Table.h"

// returns elapsed microseconds for a callable
template<typename F>
auto duration(F func) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end   = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

// prevents the compiler from eliminating unused operations
template<typename T>
inline void doNotOptimize(T* ptr) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "r,m"(ptr) : "memory");
#else
    volatile T* v = ptr;
    (void)v;
#endif
}

// Put Without Reserve
// measures insert cost including internal bucket allocation
void putNoReserve() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>          lru(counts[i]);
        std::unordered_map<int, int> map;

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j)
                lru.put(static_cast<int>(j), static_cast<int>(j));
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j)
                map.insert({static_cast<int>(j), static_cast<int>(j)});
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Put - No Reserve",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Put With Reserve
// measures insert cost with pre-allocated buckets
void putWithReserve() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(counts[i]);
        std::unordered_map<int, int> map;

        map.reserve(counts[i]);

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j)
                lru.put(static_cast<int>(j), static_cast<int>(j));
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j)
                map.insert({static_cast<int>(j), static_cast<int>(j)});
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Put - With Reserve",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Get Hit
// measures lookup cost with LRU promotion — key always present
void getHit() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(counts[i]);
        std::unordered_map<int, int> map;

        for (std::size_t j = 0; j < counts[i]; ++j) {
            lru.put(static_cast<int>(j), static_cast<int>(j));
            map.insert({static_cast<int>(j), static_cast<int>(j)});
        }

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                auto* val = lru.get(static_cast<int>(j));
                doNotOptimize(val);
            }
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                auto it = map.find(static_cast<int>(j));
                doNotOptimize(&it->second);
            }
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Get - Hit",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Get Miss
// measures lookup cost when key is always absent
void getMiss() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(counts[i]);
        std::unordered_map<int, int> map;

        for (std::size_t j = 0; j < counts[i]; ++j) {
            lru.put(static_cast<int>(j), static_cast<int>(j));
            map.insert({static_cast<int>(j), static_cast<int>(j)});
        }

        auto lru_duration = duration([&] {
            for (std::size_t j = counts[i]; j < counts[i] * 2; ++j) {
                auto* val = lru.get(static_cast<int>(j));
                doNotOptimize(val);
            }
        });

        auto map_duration = duration([&] {
            for (std::size_t j = counts[i]; j < counts[i] * 2; ++j) {
                auto it = map.find(static_cast<int>(j));
                (void)it;
            }
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Get - Miss",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Peek
// measures lookup cost without LRU promotion
void peek() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(counts[i]);
        std::unordered_map<int, int> map;

        for (std::size_t j = 0; j < counts[i]; ++j) {
            lru.put(static_cast<int>(j), static_cast<int>(j));
            map.insert({static_cast<int>(j), static_cast<int>(j)});
        }

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                const auto* val = lru.peek(static_cast<int>(j));
                doNotOptimize(const_cast<int*>(val));
            }
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                auto it = map.find(static_cast<int>(j));
                doNotOptimize(&it->second);
            }
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Peek",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Erase
// measures removal cost by key
void erase() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(counts[i]);
        std::unordered_map<int, int> map;

        for (std::size_t j = 0; j < counts[i]; ++j) {
            lru.put(static_cast<int>(j), static_cast<int>(j));
            map.insert({static_cast<int>(j), static_cast<int>(j)});
        }

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j)
                (void)lru.erase(static_cast<int>(j));
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j)
                map.erase(static_cast<int>(j));
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Erase",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Eviction
// measures insert cost at full capacity, forcing LRU eviction every insert
void eviction() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    static constexpr std::size_t CAPACITY = 1'000;

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(CAPACITY);
        std::unordered_map<int, int> map;

        for (std::size_t j = 0; j < CAPACITY; ++j) {
            lru.put(static_cast<int>(j), static_cast<int>(j));
            map.insert({static_cast<int>(j), static_cast<int>(j)});
        }

        auto lru_duration = duration([&] {
            for (std::size_t j = CAPACITY; j < CAPACITY + counts[i]; ++j)
                lru.put(static_cast<int>(j), static_cast<int>(j));
        });

        auto map_duration = duration([&] {
            for (std::size_t j = CAPACITY; j < CAPACITY + counts[i]; ++j) {
                if (map.size() >= CAPACITY)
                    map.erase(map.begin());
                map.insert({static_cast<int>(j), static_cast<int>(j)});
            }
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Eviction  (capacity = 1000)",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Contains
// measures key existence check cost
void contains() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(counts[i]);
        std::unordered_map<int, int> map;

        for (std::size_t j = 0; j < counts[i]; ++j) {
            lru.put(static_cast<int>(j), static_cast<int>(j));
            map.insert({static_cast<int>(j), static_cast<int>(j)});
        }

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                bool found = lru.contains(static_cast<int>(j));
                doNotOptimize(&found);
            }
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                bool found = map.contains(static_cast<int>(j));
                doNotOptimize(&found);
            }
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Contains",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Mixed Workload
// measures interleaved put, get, and eviction under realistic access patterns
void mixedWorkload() {
    std::vector<long> lru_durations;
    std::vector<long> map_durations;

    std::vector<std::size_t> counts = {
        100'000,
        200'000,
        400'000,
        800'000
    };

    static constexpr std::size_t CAPACITY = 1'000;

    for (std::size_t i = 0; i < counts.size(); ++i) {
        LRUCache<int, int>           lru(CAPACITY);
        std::unordered_map<int, int> map;

        auto lru_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                int key = static_cast<int>(j % (CAPACITY * 2));

                lru.put(key, static_cast<int>(j));

                auto* val = lru.get(key / 2);
                doNotOptimize(val);
            }
        });

        auto map_duration = duration([&] {
            for (std::size_t j = 0; j < counts[i]; ++j) {
                int key = static_cast<int>(j % (CAPACITY * 2));

                if (map.size() >= CAPACITY)
                    map.erase(map.begin());

                map.insert_or_assign(key, static_cast<int>(j));

                auto it = map.find(key / 2);
                if (it != map.end())
                    doNotOptimize(&it->second);
            }
        });

        lru_durations.push_back(lru_duration.count());
        map_durations.push_back(map_duration.count());
    }

    std::vector<std::vector<std::string>> data{
        Table::convert(counts),
        Table::convert(lru_durations, "us"),
        Table::convert(map_durations, "us")
    };

    Table::table(
        "Mixed Workload  (capacity = 1000)",
        { "Count", "LRUCache", "unordered_map" },
        data, 56);
}

// Entry Point
int main() {
    putNoReserve();
    putWithReserve();
    getHit();
    getMiss();
    peek();
    erase();
    eviction();
    contains();
    mixedWorkload();

    return 0;
}
