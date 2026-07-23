# LRUCache

<p align="center">
  <img src="https://img.shields.io/github/v/release/privateMwb/LRUCache?style=for-the-badge&logo=github&color=yellow" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-orange?style=for-the-badge" alt="License - MIT">
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue?style=for-the-badge&logo=c%2B%2B" alt="C++ - 23">
</p>

<p align="center">
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/build.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/build.yml/badge.svg" alt="Build and Test">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/benchmark.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/benchmark.yml/badge.svg" alt="Benchmarks">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/coverage.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/coverage.yml/badge.svg" alt="Coverage">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/sanitizers.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/sanitizers.yml/badge.svg" alt="Sanitizers">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/clang-tidy.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/clang-tidy.yml/badge.svg" alt="Clang Tidy">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/clang-format.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/clang-format.yml/badge.svg" alt="Clang Format">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/docs.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/docs.yml/badge.svg" alt="Documentation">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/release.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/release.yml/badge.svg" alt="Release">
  </a>
  <a href="https://github.com/privateMwb/LRUCache/actions/workflows/packaging.yml">
    <img src="https://github.com/privateMwb/LRUCache/actions/workflows/packaging.yml/badge.svg" alt="Packaging">
  </a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/GCC-support-B46F1B?style=flat&logo=gnu" alt="GCC - support">
  <img src="https://img.shields.io/badge/Clang-support-045891?style=flat&logo=llvm" alt="Clang - support">
  <img src="https://img.shields.io/badge/MSVC-support-5C2D91?style=flat" alt="MSVC - support">
  <img src="https://img.shields.io/badge/AppleClang-support-000000?style=flat&logo=apple" alt="AppleClang - support">
</p>

LRUCache is a header-only, fixed-capacity LRU cache for modern C++ — O(1) get/put/erase, a pool-allocated intrusive list instead of `std::list`'s per-node heap allocations, and a flat open-addressing table instead of `std::unordered_map`'s chaining, so steady-state operation performs zero heap allocations at all.

## 📑 Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Development](#development)
- [Benchmarks](#benchmarks)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [Changelog](#changelog)
- [License](#license)

## <a id="features"></a>✨ Features

- **Pool-allocated intrusive list** — every node lives in one array allocated once at construction, linked into recency order via an embedded `Link` base rather than a separate list-node allocation. Eviction returns a slot to a free list instead of freeing memory, so steady-state `put()`/`get()`/eviction never touches the heap.
- **Open-addressing table with backward-shift deletion** — key lookup is one flat array probed linearly, not a chain of heap-allocated buckets. Each slot stores `{hash, Node*}`, not the key itself, so nothing is duplicated between the table and the pool. Deletion shifts subsequent entries back rather than leaving a tombstone, so probe sequences never degrade over the cache's lifetime the way lazy-deletion open addressing does.
- **Slot-caching nodes** — every node remembers its own table index, so eviction never re-hashes or re-probes to find its table entry.
- **Strong exception guarantee on insert** — a node is fully constructed before it's linked into the table or list, so a throwing `K`/`V` constructor leaves the cache exactly as it was. `emplace()` forwards its arguments straight into the value's constructor in place, with no intermediate temporary to also worry about.
- **Real usage statistics** — `hitCount()`/`missCount()`/`hitRate()` are tracked internally, not bolted on by wrapping every call site yourself.
- **Deliberate control over the memory footprint** — `resize()` grows or shrinks capacity (evicting if needed), `shrink_to_fit()` reclaims unused pool/table space down to the current size, and `reserve()` is an honest no-op (documented as such) rather than a silent copy of `std::unordered_map`'s API for a design that doesn't need it.

## <a id="requirements"></a>📋 Requirements

- A C++23-conformant compiler (tested: GCC, Clang, MSVC, AppleClang)
- CMake 3.20+

## <a id="installation"></a>📦 Installation

**From source:**

```bash
git clone https://github.com/privateMwb/LRUCache.git
cd LRUCache
cmake -B build \
  -DBUILD_TESTS=OFF \
  -DBUILD_BENCHMARKS=OFF \
  -DBUILD_REGRESSION=OFF \
  -DBUILD_EXAMPLES=OFF
cmake --install build
```

Then, in your own `CMakeLists.txt`:

```cmake
find_package(CachePro CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE CachePro::CachePro)
```

> vcpkg and Conan packages are built and verified (recipe in
> `packaging/recipes/cachepro/`, port in `packaging/vcpkg/ports/cachepro/`),
> but not yet published to the public registries. This section will be
> updated once they are.

## <a id="quick-start"></a>🚀 Quick Start

```cpp
#include <CachePro/LRUCache.h>

int main() {
    CachePro::LRUCache<std::string, int> cache(3);

    cache.put("alpha", 1);
    cache.put("beta", 2);
    cache.put("gamma", 3);

    if (int* value = cache.get("alpha")) {
        // hit — "alpha" is now the most recently used entry
    }

    cache.put("delta", 4); // cache is full — evicts "beta", the current LRU entry
}
```

Building a value in place, and reading back hit/miss statistics:

```cpp
CachePro::LRUCache<int, std::string> cache(100);

auto& value = cache.emplace(42, "constructed in place, no temporary");

std::cout << cache.hitCount() << " hits, " << cache.missCount() << " misses\n";
std::cout << cache.hitRate() << "% hit rate\n";
```

Construction enforces a real invariant rather than silently clamping:

```cpp
try {
    CachePro::LRUCache<int, int> cache(0); // capacity must be > 0
} catch (const std::invalid_argument& e) {
    std::cerr << e.what() << '\n';
}
```

## <a id="project-structure"></a>🗂️ Project Structure

```
LRUCache/
├── include/
│   └── CachePro/
│       ├── LRUCache.h
│       ├── LRUCache.tpp
│       └── Node.h
│
├── tests/
│   ├── support/
│   ├── suite/
│   ├── test_main.cpp
│   └── CMakeLists.txt
│
├── benchmarks/
│   ├── support/
│   ├── suite/
│   ├── baselines/
│   ├── bench_main.cpp
│   └── CMakeLists.txt
│
├── examples/
│   ├── support/
│   ├── suite/
│   ├── example_main.cpp
│   └── CMakeLists.txt
│
├── regression/
│   ├── support/
│   ├── regression_main.cpp
│   └── CMakeLists.txt
│
├── packaging/
│   ├── README.md
│   ├── recipes/
│   │   └── cachepro/
│   ├── vcpkg/
│   │   └── ports/
│   │       └── cachepro/
│   └── vcpkg-smoke-test/
│
├── scripts/
│   └── update_package_files.py
│
├── .github/
│   ├── releases/
│   └── workflows/
│
├── cmake/
│   └── CacheProConfig.cmake.in
│
├── docs/
│   ├── Doxyfile
│   └── README.md
│
├── .gitignore
├── CMakeLists.txt
├── README.md
└── LICENSE
```

## <a id="development"></a>🛠️ Development

The from-source install above builds the library only. To work on
CachePro itself — running tests, benchmarks, or the regression tool —
build with everything enabled (the default):

```bash
cmake -B build
cmake --build build
```

**Run the test suite:**

```bash
ctest --test-dir build
```

**Run benchmarks and check for regressions:**

```bash
./build/benchmarks
./build/regression                  # latest baseline vs. benchmarks/results/benchmark_results.json
./build/regression v1.2.0           # a specific baseline vs. current
./build/regression v1.2.0 v1.4.0    # two baselines against each other
```

`regression` picks the latest baseline by semantic version (`v1.10.0`
correctly outranks `v1.9.0`), not alphabetical filename order, and
auto-names its output (`regression_v1.2.0_vs_current.md`/`.json`, etc.).

See [docs/README.md](docs/README.md) for notes on verifying the vcpkg
port and Conan recipe locally.

## <a id="benchmarks"></a>📊 Benchmarks

Measured against `stdLRU` (`std::list` + `std::unordered_map`), same
build, at 10K / 100K / 1M iterations (`benchmarks/baselines/v1.0.0.json`
has the full dataset).

| Operation | CachePro (1M) | stdLRU (1M) | Δ |
|---|---|---|---|
| `Reserve()` | 627.44 us | 12.84 ms | +1946.3% |
| `Clear() + Refill` | 868.47 ms | 4.21 s | +384.5% |
| `Move-construct` | 1.02 s | 4.63 s | +353.2% |
| `Contains() Miss` | 1.24 ms | 5.61 ms | +351.9% |
| `Get() Miss` | 2.48 ms | 4.97 ms | +100.4% |
| `Emplace() Insert` | 40.63 ms | 74.86 ms | +84.2% |
| `Put() Insert` | 41.51 ms | 45.48 ms | +9.6% |
| `Get() Hit` | 2.20 ms | 1.89 ms | -14.2% |
| `Resize() Grow` | 24.79 ms | 17.55 ms | -29.2% |
| `Shrink To Fit()` | 1.52 s | 521.45 ms | -65.8% |
| `Construct Empty` | 3.30 s | 5.36 ms | -99.8% |

CachePro's pool-based, open-addressing design pays off most on
miss-heavy workloads (`Contains()`/`Get()` misses), bulk churn
(`Clear()` + refill), and `Reserve()` — a genuine no-op here versus a
real rehash in `stdLRU`.

The trade-off: the pool and table are allocated once, eagerly, at
construction — so `Construct Empty` is consistently slower than
`stdLRU`'s lazy allocation, and calls that trigger a reallocation
(`Resize() Grow`, `Shrink To Fit()`) pay that cost directly rather than
amortizing it.

## <a id="documentation"></a>📖 Documentation

Full API reference, generated with Doxygen from `docs/Doxyfile`:

**https://privateMwb.github.io/LRUCache/**

## <a id="contributing"></a>🤝 Contributing

Issues and pull requests are welcome. Before submitting a PR:

- Run the test suite (`ctest --test-dir build`)
- If you're changing a hot path, run `./build/regression` and mention
  the results in your PR description

## <a id="changelog"></a>📝 Changelog

See the [Releases](https://github.com/privateMwb/LRUCache/releases)
page for version history and release notes.

## <a id="license"></a>📄 License

MIT — see [LICENSE](LICENSE) for details.
