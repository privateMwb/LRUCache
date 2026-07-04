# LRUCache

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://en.cppreference.com/w/cpp/23)
[![Status](https://img.shields.io/badge/status-learning%20project-green)](https://github.com/privateMwb/CachePro)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

**LRUCache** is a from-scratch, high-performance Least Recently Used (LRU) cache written in modern C++23. It was built as a deep dive into intrusive data structure design — pool-allocated nodes, a free-list-backed doubly linked list, single-probe hash table operations, and exception-safe insert/evict paths.

---

## Table of Contents

- [Overview](#overview)
- [Motivation](#motivation)
- [Features](#features)
- [Quick Start](#quick-start)
- [Core API](#core-api)
- [Design Overview](#design-overview)
- [Complexity](#complexity)
- [Benchmarks](#benchmarks)
- [Project Structure](#project-structure)
- [Building from Source](#building-from-source)
- [Known Limitations](#known-limitations)
- [License](#license)

---

## Overview

`CachePro::LRUCache` is a fixed-capacity cache combining an `std::unordered_map` for O(1) key lookup with an intrusive doubly linked list for O(1) recency tracking. It focuses on correctly and efficiently solving the hard problems a production LRU cache faces internally:

- Zero heap allocation on the steady-state insert/evict path
- Single hash probe per `put`/`get`/`erase` call
- Exception-safe insertion with map-first commit ordering
- Support for non-default-constructible and move-only value types

On top of this foundation, CachePro adds runtime resizing, hit/miss statistics, and order-inspection utilities not found in typical hand-rolled LRU implementations.

---

## Motivation

This project was built to understand, in depth:

- Intrusive linked-list design and sentinel-free list management
- Pool allocation with a free-list for O(1) node acquire/release
- Eliminating redundant hashing across combined lookup+mutate operations (`try_emplace` vs. find-then-insert)
- Exception safety — committing to the hash map before linking into the list, so a throwing constructor can't corrupt cache state
- Node layout that never requires `K`/`V` to be default-constructible
- Rigorous performance benchmarking against a textbook `unordered_map` + `std::list` LRU baseline

---

## Features

| Feature | Description |
|---|---|
| Pool-allocated nodes | Fixed-capacity node pool with an intrusive free-list; no `new`/`delete` on the steady-state insert/evict path |
| Single-probe hot paths | `put`, `get`, and `erase` each perform exactly one hash lookup via `try_emplace`/iterator reuse |
| Intrusive linked list | Sentinel-free `Link` base struct; no default-constructed `K`/`V` required for list bookkeeping |
| Exception-safe insertion | Hash map insertion commits before the node is linked, so a throwing constructor can't leave a dangling list entry |
| Move-only & non-default-constructible values | Nodes are always constructed in place; never require `V` to be default-constructible |
| Runtime resizing | `resize()` grows or shrinks capacity, evicting down to the new size when shrinking |
| Hit/miss statistics | `hitCount()`, `missCount()`, `hitRate()`, `resetStats()` for cache-effectiveness monitoring |
| Order inspection | `keys()`, `mostRecentKey()`, `leastRecentKey()` for debugging and introspection |
| `emplace()` support | In-place value construction, avoiding an intermediate temporary |
| Move semantics | Full move construction/assignment; copying is explicitly disabled |

---

## Quick Start

### Basic usage

```cpp
#include "LRUCache.h"

using namespace CachePro;

int main() {
    LRUCache<std::string, int> cache(3);

    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    if (int* v = cache.get("a")) {
        // hit: promotes "a" to most-recently-used
    }
}
```

### Eviction

```cpp
#include "LRUCache.h"

using namespace CachePro;

int main() {
    LRUCache<std::string, int> cache(2);

    cache.put("x", 1);
    cache.put("y", 2);
    cache.put("z", 3);   // evicts "x" (least-recently-used)
}
```

### Statistics monitoring

```cpp
#include "LRUCache.h"

using namespace CachePro;

int main() {
    LRUCache<std::string, int> cache(100);

    cache.get("missing"); // miss
    cache.put("key", 1);
    cache.get("key");     // hit

    double rate = cache.hitRate(); // 50.0
}
```

### Runtime resizing

```cpp
#include "LRUCache.h"

using namespace CachePro;

int main() {
    LRUCache<std::string, int> cache(2);
    cache.put("a", 1);
    cache.put("b", 2);

    cache.resize(1); // evicts "a", keeps "b"
}
```

---

## Core API

### Constructors

```cpp
explicit LRUCache(std::size_t capacity);   // throws std::invalid_argument if capacity == 0
LRUCache(LRUCache&& other) noexcept;
LRUCache& operator=(LRUCache&& other) noexcept;
// Copy construction/assignment are deleted.
```

### Modifiers

```cpp
void put(const K& key, const V& value);
void put(const K& key, V&& value);
void put(K&& key, const V& value);
void put(K&& key, V&& value);

template<typename... Args>
V& emplace(const K& key, Args&&... args);

[[nodiscard]] bool erase(const K& key);
void clear() noexcept;
```

### Lookup

```cpp
[[nodiscard]] V* get(const K& key);              // promotes to most-recently-used
[[nodiscard]] const V* peek(const K& key) const; // no promotion, no stats
[[nodiscard]] bool contains(const K& key) const;
```

### Capacity

```cpp
void resize(std::size_t newCapacity); // shrinks evict down to newCapacity
void reserve(std::size_t count);

[[nodiscard]] std::size_t capacity() const noexcept;
[[nodiscard]] std::size_t size() const noexcept;
[[nodiscard]] bool empty() const noexcept;
```

### Inspection

```cpp
[[nodiscard]] std::vector<K> keys() const;        // most-recent -> least-recent
[[nodiscard]] const K* mostRecentKey() const noexcept;
[[nodiscard]] const K* leastRecentKey() const noexcept;
```

### Statistics

```cpp
[[nodiscard]] std::size_t hitCount() const noexcept;
[[nodiscard]] std::size_t missCount() const noexcept;
[[nodiscard]] double hitRate() const noexcept;
void resetStats() noexcept;
```

---

## Design Overview

`LRUCache` combines a pool-allocated intrusive doubly linked list with an `std::unordered_map<K, Node<K,V>*>` for lookup.

### Internal layout

```
storage_ (pool buffer, capacity_ * sizeof(Node<K,V>))
  ↓
[Node][Node][Node][...][Node]     <- reused via freeHead_ free-list
                                       (never returned to the OS until destruction/resize)

head_ <-> Node <-> Node <-> ... <-> Node <-> tail_
(most recently used)                      (least recently used)

cache_: unordered_map<K, Node<K,V>*>   <- O(1) lookup, single probe per operation
```

- **`Link`** — a minimal, non-templated `{prev, next}` base. `head_`/`tail_` are plain `Link` members (not heap-allocated, not `Node<K,V>` sentinels), so `K`/`V` are never required to be default-constructible.
- **Node pool** — `capacity_` nodes are allocated once as raw storage. Eviction and erase return a node's slot to `freeHead_` instead of calling `delete`; the next `put` reuses that slot via `std::construct_at`, avoiding allocator calls entirely in steady state.
- **Single-probe operations** — `put` uses `try_emplace` and reuses its iterator for both the "already exists" and "insert new" branches. `erase` uses `find` once and erases by iterator. This halves the hash/probe cost versus a find-then-insert/erase-by-key pattern.
- **Commit ordering** — the hash map entry is created before the node is linked into the list, so if node construction throws (e.g. `V`'s constructor), the cache is left in its prior valid state rather than with an untracked list node.

---

## Complexity

| Operation | Time | Notes |
|---|---|---|
| `put` | O(1) amortized | One hash probe, one pool acquire/reuse |
| `get` / `peek` / `contains` | O(1) | One hash probe |
| `erase` | O(1) | One hash probe, one pool release |
| `emplace` | O(1) amortized | One hash probe, in-place construction |
| `clear` | O(n) | Destroys and releases every live node |
| `resize` (grow) | O(n) | Reallocates pool, relinks all nodes |
| `resize` (shrink) | O(n) | Evicts down to new capacity, then reallocates pool |
| `keys` | O(n) | Full list traversal |
| `mostRecentKey` / `leastRecentKey` | O(1) | Direct sentinel-adjacent access |

---

## Benchmarks

Measured against a naive `unordered_map<K, list-iterator>` + `std::list<pair<K,V>>` LRU baseline — the textbook implementation this library is meant to outperform.

<details>
<summary>Click to expand full benchmark results</summary>

#### Put

```
----------------------------------------------------------------------
Put                                     Time           Iteration
----------------------------------------------------------------------
CachePro put copy                       191.16 ms       1000
Naive LRU put copy                      332.83 ms       1000

CachePro put move                       243.15 ms       1000
Naive LRU put move                      378.87 ms       1000

CachePro put update                     1.59 ms         100000
Naive LRU put update                    1.62 ms         100000

CachePro put (full, evicts)             42.13 ms        100000
Naive LRU put (full, evicts)            75.98 ms        100000
----------------------------------------------------------------------
```

#### Get

```
----------------------------------------------------------------------
Get                                     Time           Iteration
----------------------------------------------------------------------
CachePro get hit (relink)               4.95 ms         1000000
Naive LRU get hit (relink)              4.23 ms         1000000

CachePro get hit (already MRU)          6.88 ms         1000000
Naive LRU get hit (already MRU)         5.42 ms         1000000

CachePro get miss                       14.74 ms        1000000
Naive LRU get miss                      14.00 ms        1000000

CachePro peek (no reorder)              5.69 ms         1000000
CachePro get (reorder)                  7.40 ms         1000000

CachePro contains                       5.29 ms         1000000
Naive LRU contains                      5.84 ms         1000000
----------------------------------------------------------------------
```

#### Eviction

```
----------------------------------------------------------------------
Eviction                                Time           Iteration
----------------------------------------------------------------------
CachePro evict steady (cap 100)         206.84 ms       1000000
Naive LRU evict steady (cap 100)        813.18 ms       1000000

CachePro evict steady (cap 100K)        528.58 ms       1000000
Naive LRU evict steady (cap 100K)       809.77 ms       1000000

CachePro single eviction                336.74 ms       500000
Naive LRU single eviction               404.44 ms       500000

CachePro fill/clear/refill              183.51 ms       1000
Naive LRU fill/clear/refill             336.81 ms       1000
----------------------------------------------------------------------
```

#### Erase

```
----------------------------------------------------------------------
Erase                                   Time           Iteration
----------------------------------------------------------------------
CachePro erase existing                 184.22 ms       1000
Naive LRU erase existing                334.24 ms       1000

CachePro erase missing                  27.46 ms        1000000
Naive LRU erase missing                 27.47 ms        1000000

CachePro erase MRU                      189.38 ms       1000
Naive LRU erase MRU                     338.82 ms       1000

CachePro erase LRU                      190.70 ms       1000
Naive LRU erase LRU                     337.65 ms       1000

CachePro erase/reinsert cycle           201.19 ms       1000000
Naive LRU erase/reinsert cycle          301.78 ms       1000000
----------------------------------------------------------------------
```

#### Emplace

```
----------------------------------------------------------------------
Emplace                                 Time           Iteration
----------------------------------------------------------------------
CachePro emplace new                    218.35 ms       1000
CachePro put (temporary)                205.46 ms       1000
Naive LRU put (temporary)               340.10 ms       1000

CachePro emplace existing               25.08 ms        1000000
Naive LRU put existing                  21.39 ms        1000000

CachePro emplace multi-arg              181.87 ms       1000
CachePro put (constructed temp)         180.14 ms       1000

CachePro emplace (full, evicts)         443.95 ms       1000000
Naive LRU put (full, evicts)            767.44 ms       1000000
----------------------------------------------------------------------
```

#### Resize

```
----------------------------------------------------------------------
Resize                                  Time           Iteration
----------------------------------------------------------------------
CachePro resize grow 1K->2K             221.96 ms       1000
Naive LRU manual grow 1K->2K            671.57 ms       1000

CachePro resize shrink 2K->500          444.63 ms       1000
Naive LRU manual shrink 2K->500         849.64 ms       1000

CachePro resize no-op shrink            109.47 ms       1000

CachePro resize grow/shrink cycle       135.85 ms       1000
----------------------------------------------------------------------
```

#### Mixed Workload

```
----------------------------------------------------------------------
Mixed Workload                          Time           Iteration
----------------------------------------------------------------------
CachePro 80/20 uniform                  675.18 ms       1000
Naive LRU 80/20 uniform                 944.84 ms       1000

CachePro 95/5 uniform                   280.05 ms       1000
Naive LRU 95/5 uniform                  344.51 ms       1000

CachePro 80/20 zipf                     277.57 ms       1000
Naive LRU 80/20 zipf                    316.51 ms       1000

CachePro 50/50 small cap                1.55 s          1000
Naive LRU 50/50 small cap               2.02 s          1000
----------------------------------------------------------------------
```

#### Scaling

```
----------------------------------------------------------------------
Scaling                                 Time           Iteration
----------------------------------------------------------------------
Fill from empty
CachePro fill 1K                        189.74 ms       1000
Naive LRU fill 1K                       338.14 ms       1000
CachePro fill 10K                       256.92 ms       100
Naive LRU fill 10K                      539.97 ms       100
CachePro fill 100K                      339.76 ms       10
Naive LRU fill 100K                     824.56 ms       10
CachePro fill 1M                        156.62 ms       1
Naive LRU fill 1M                       250.73 ms       1

Steady-state get
CachePro get @1K                        7.96 ms         1000000
CachePro get @100K                      6.93 ms         1000000
CachePro get @1M                        7.45 ms         1000000
Naive LRU get @1K                       5.86 ms         1000000
Naive LRU get @100K                     5.44 ms         1000000
Naive LRU get @1M                       5.87 ms         1000000

Steady-state evict
CachePro evict @1K                      426.62 ms       1000000
CachePro evict @100K                    460.53 ms       1000000
CachePro evict @1M                      446.54 ms       1000000
Naive LRU evict @1K                     765.25 ms       1000000
Naive LRU evict @100K                   872.46 ms       1000000
Naive LRU evict @1M                     793.05 ms       1000000
----------------------------------------------------------------------
```

</details>

---

## Project Structure

```
LRUCache/
├── include/
│   └── CachePro/
│       ├── Node.h
│       ├── LRUCache.h
│       └── LRUCache.tpp
│
├── tests/
├── benchmarks/
├── examples/
│
├── cmake/
│   └── CacheProConfig.cmake.in
│
├── .gitignore
├── CMakeLists.txt
├── README.md
└── LICENSE
```

---

## Building from Source

### Requirements

- GCC 13+ or Clang with C++23 support
- CMake 3.20+

### Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Run tests

```bash
./tests
```

### Run benchmarks

```bash
./benchmarks
```

### Run examples

```bash
./examples
```

---

## Known Limitations

- **`put`/`get` on an already-warm cache is sometimes slower than the naive baseline** on raw single-operation benchmarks (e.g. `get hit (already MRU)`: 6.88 ms vs. 5.42 ms). The pool/intrusive-list design wins decisively on allocation-heavy workloads (put, evict, erase, resize — often 1.5–3x faster), but the pure-pointer-chase hit path shows no advantage yet; root cause not isolated.
- **Self move assignment is unguarded.** `operator=(LRUCache&&)` does not check `this != &other` before releasing internal state, so `c = std::move(c)` is unsafe. A guard is a planned fix.
- **No heterogeneous/transparent lookup.** `get`/`put`/`erase` all require an exact `K`, so `LRUCache<std::string, V>` can't be queried with `std::string_view` without an implicit conversion/allocation.
- **No allocator or PMR support**, despite the internal design (raw pool buffer) being a natural fit for one.
- **`emplace()` only accepts `const K&`** — no rvalue-key overload, so move-only or expensive-to-copy keys can't use the in-place construction path.

### Fixed during development

- `Node.h` previously had a namespace block that didn't enclose the `Node` struct, which would not compile.
- `put()`/`erase()` previously performed two hash probes (a `find` followed by a separate `[]`/`erase(key)`); both now reuse a single iterator.
- `put(K&&, V&&)` previously copied the key unconditionally before moving it, defeating the purpose of the rvalue overload.
- Sentinel nodes previously required `K`/`V` to be default-constructible; the list now uses a non-templated `Link` base with no payload.
- Node allocation previously called `new`/`delete` per insert/evict; nodes are now pool-allocated with free-list reuse.
- `resize()` previously threw when shrinking below the current size instead of evicting down to the new capacity.
- Map insertion previously happened after list insertion in `put()`, so a throwing constructor could leave an untracked node linked into the list; map insertion now commits first.

---

## License

Licensed under the [MIT License](LICENSE) — free to use, modify, and distribute for educational and personal purposes.
