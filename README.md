# LRUCache

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://en.cppreference.com/w/cpp/23)
[![Status](https://img.shields.io/badge/status-learning%20project-green)](https://github.com/privateMwb/LRUCache)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A Least Recently Used (LRU) cache implemented from scratch in **C++23**, built to explore doubly-linked list management, hash map integration, eviction policy design, and the tradeoffs between raw lookup speed and ordering overhead.

---

## Table of Contents

- [Overview](#overview)
- [Motivation](#motivation)
- [Features](#features)
- [Design Overview](#design-overview)
  - [Internal Structure](#internal-structure)
  - [The Two-Layer Design](#the-two-layer-design)
  - [Sentinel Nodes](#sentinel-nodes)
  - [LRU Ordering](#lru-ordering)
  - [Eviction Policy](#eviction-policy)
  - [get vs peek](#get-vs-peek)
  - [put Overloads](#put-overloads)
  - [Statistics Tracking](#statistics-tracking)
  - [Memory Management](#memory-management)
- [Complexity](#complexity)
- [Quick Example](#quick-example)
  - [Basic Usage](#basic-usage)
  - [Cache Miss Fallback](#cache-miss-fallback)
  - [peek vs get](#peek-vs-get)
  - [Eviction Policy](#eviction-policy-1)
  - [Memoization](#memoization)
  - [Cache Statistics](#cache-statistics)
  - [Move Ownership](#move-ownership)
- [Real-World Use Cases](#real-world-use-cases)
- [Core API](#core-api)
  - [Constructor & Destructor](#constructor--destructor)
  - [Cache Modifiers](#cache-modifiers)
  - [Cache Lookup](#cache-lookup)
  - [Capacity Management](#capacity-management)
  - [Element Access](#element-access)
  - [Cache Statistics](#cache-statistics-1)
  - [Capacity & State](#capacity--state)
- [Benchmark Results](#benchmark-results)
  - [Put — No Reserve](#put--no-reserve)
  - [Put — With Reserve](#put--with-reserve)
  - [Get — Hit](#get--hit)
  - [Get — Miss](#get--miss)
  - [Peek](#peek)
  - [Erase](#erase)
  - [Eviction](#eviction)
  - [Contains](#contains)
  - [Mixed Workload](#mixed-workload)
  - [Summary](#summary)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Notes](#notes)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

LRUCache is a fixed-capacity cache that evicts the least recently used entry when full. It is built on two cooperating data structures: a doubly-linked list that maintains access order, and an `std::unordered_map` that provides O(1) key lookup. Every `get()` promotes the accessed node to the front of the list; every eviction removes the node at the back.

It also includes `peek()` for read-only access without affecting order, a `keys()` snapshot in recency order, `mostRecentKey()` / `leastRecentKey()` introspection, and a statistics system tracking hits, misses, and hit rate.

---

## Motivation

This project was built to deeply understand:

- The classic LRU cache design pattern and why it requires two data structures
- Doubly-linked list manipulation (insert, remove, move-to-front, pop-back)
- Sentinel node technique to eliminate edge cases in list operations
- Manual heap management (`new`, `delete`, raw pointer ownership)
- The Rule of 5 — and why copy is deleted while move is supported
- Hash map + linked list integration for O(1) access and O(1) eviction
- The real cost of LRU ordering overhead compared to plain hash map lookup
- Hit rate tracking and cache effectiveness measurement

---

## Features

- Fixed-capacity LRU eviction policy
- `put()` — insert or update; promotes to most-recent on update
- `get()` — lookup with LRU promotion; returns `nullptr` on miss
- `peek()` — read-only lookup without affecting recency order
- `erase()` — explicit removal by key
- `clear()` — remove all entries, retain capacity
- `resize()` — increase capacity limit
- `contains()` — O(1) existence check without side effects
- `keys()` — snapshot of all keys in most-recent-first order
- `mostRecentKey()` / `leastRecentKey()` — O(1) recency introspection
- Hit/miss statistics: `hitCount()`, `missCount()`, `hitRate()`, `resetStats()`
- Three `put()` overloads for copy, move-value, and full move
- Move construction and move assignment (copy is deleted)
- Custom hash function support via template parameter

---

## Design Overview

### Internal Structure

```
head (sentinel)
  ↓
[ Node: key=3, val="C" ] ← most recent
  ↓
[ Node: key=1, val="A" ]
  ↓
[ Node: key=2, val="B" ] ← least recent (evicted next)
  ↓
tail (sentinel)

cache (unordered_map)
  key=1 → Node*
  key=2 → Node*
  key=3 → Node*
```

Each `Node<K, V>` holds `key`, `value`, `prev*`, and `next*`. The `cache` map stores raw pointers to nodes — giving O(1) access to any node's position in the list without traversal.

---

### The Two-Layer Design

Neither data structure alone is sufficient:

| Structure alone         | Problem                                      |
|-------------------------|----------------------------------------------|
| Linked list only        | O(n) lookup — must scan to find a key        |
| Hash map only           | No ordering — can't identify the LRU entry   |
| **Both together**       | O(1) lookup via map + O(1) order via list    |

The map holds `key → Node*`. The list holds nodes in recency order. Every operation touches both: the map for O(1) lookup, the list for O(1) reordering.

---

### Sentinel Nodes

`head` and `tail` are permanent dummy nodes that never hold real data:

```
head ↔ [real nodes...] ↔ tail
```

This eliminates all null-pointer edge cases from list operations. Inserting at front is always `head->next = node`; popping from back is always `tail->prev`. No special handling needed for empty cache or single-element cache.

---

### LRU Ordering

The list is maintained in most-recent-first order:

```
head ↔ [most recent] ↔ ... ↔ [least recent] ↔ tail
```

Three internal operations maintain this invariant:

- `insertFront(node)` — links node immediately after `head`
- `removeNode(node)` — unlinks node from its current position
- `moveToFront(node)` — `removeNode` + `insertFront` (O(1) pointer rewiring)
- `popBack()` — unlinks and returns `tail->prev` (the LRU entry)

All four are O(1) pure pointer operations — no traversal, no allocation.

---

### Eviction Policy

When `put()` is called on a full cache:

```
1. popBack()          → unlink the LRU node from the list
2. cache.erase(lru->key) → remove it from the hash map
3. delete lru         → free the node
4. new Node(key, val) → allocate the new node
5. insertFront(node)  → link it as most recent
6. cache[key] = node  → register in the hash map
```

The evicted key is always `leastRecentKey()` before the insertion. Any node can be protected from eviction by accessing it with `get()` — which promotes it to the front — before the cache fills.

---

### get vs peek

| Method   | LRU order affected | Hit/miss counted | Returns       |
|----------|--------------------|------------------|---------------|
| `get()`  | ✅ promotes to front | ✅ yes           | `V*` or `nullptr` |
| `peek()` | ❌ no change        | ❌ no            | `const V*` or `nullptr` |

`peek()` is useful when you need to inspect a value without "using" it — for example, checking cache state in diagnostics, or reading a value that shouldn't count as a recent access.

---

### put Overloads

Three overloads handle different ownership scenarios:

```cpp
put(const K& key, const V& value)  // copy both
put(const K& key, V&& value)       // copy key, move value
put(K&& key, V&& value)            // move both
```

For the full-move overload, a `keyCopy` is made before moving the key into the node — this is necessary because the map insertion needs the key after it has been moved into the node.

If the key already exists, all three overloads update the value in-place and promote the node to most-recent. No eviction occurs on update.

---

### Statistics Tracking

`hitCounter` and `missCounter` are plain `std::size_t` members incremented by `get()`:

- Hit: key found → `++hitCounter`, node promoted, returns `&node->value`
- Miss: key not found → `++missCounter`, returns `nullptr`

`peek()`, `contains()`, and `find()` do **not** affect statistics — only `get()` does.

`hitRate()` returns `(hits / total) * 100.0` as a `double` percentage. Returns `0.0` if no accesses have been recorded yet.

---

### Memory Management

LRUCache owns all `Node` objects:

- **Construction:** allocates two sentinel nodes (`head`, `tail`); linked together
- **`put()` (new key):** `new Node<K,V>(key, value)` per entry
- **`erase()` / eviction:** `delete node` for the removed node
- **`clear()`:** deletes all real nodes; sentinels retained; `cache` cleared
- **`release()`:** deletes sentinels too; called by destructor and move assignment
- **Move:** transfers raw pointers, zeros out source — O(1), no allocation
- **Copy:** deleted — copying a linked structure with raw pointers safely requires deep copy logic that was intentionally omitted to keep ownership unambiguous

---

## Complexity

| Operation           | Time     | Notes                                              |
|---------------------|:--------:|----------------------------------------------------|
| `put()` (new key)   | O(1) avg | Hash map insert + list prepend + optional eviction |
| `put()` (existing)  | O(1) avg | In-place value update + moveToFront                |
| `get()`             | O(1) avg | Hash map lookup + moveToFront                      |
| `peek()`            | O(1) avg | Hash map lookup only — no list modification        |
| `erase()`           | O(1) avg | Hash map erase + list unlink + delete              |
| `contains()`        | O(1) avg | Hash map lookup only                               |
| `clear()`           | O(n)     | Deletes all nodes; traverses the list              |
| `keys()`            | O(n)     | Traverses list front-to-back                       |
| `mostRecentKey()`   | O(1)     | `head->next->key`                                  |
| `leastRecentKey()`  | O(1)     | `tail->prev->key`                                  |
| `resize()`          | O(1)     | Updates capacity field only                        |
| Move construction   | O(1)     | Pointer transfer                                   |
| Destructor          | O(n)     | Deletes all nodes + sentinels                      |

> Average-case O(1) for hash map operations assumes a well-distributed hash function and low load factor.

---

## Quick Example

### Basic Usage

```cpp
#include "LRUCache.h"
#include <iostream>

int main() {
    LRUCache<int, std::string> lru(3);

    lru.put(1, "Apple");
    lru.put(2, "Banana");
    lru.put(3, "Mango");

    auto* item = lru.get(1);
    if (item) std::cout << *item << "\n"; // Apple

    std::cout << lru.size()     << "\n"; // 3
    std::cout << lru.capacity() << "\n"; // 3
}
```

---

### Cache Miss Fallback

The classic pattern: check the cache first, fall back to a data source on miss.

```cpp
LRUCache<int, std::string> lru(5);

auto fetch = [&](int id) -> std::string {
    auto* cached = lru.get(id);
    if (cached) return *cached;

    // cache miss — fetch from database
    std::string result = "User_" + std::to_string(id);
    lru.put(id, result);
    return result;
};

fetch(1); // miss — fetches from DB, caches result
fetch(1); // hit  — returned from cache
fetch(2); // miss
fetch(1); // hit
```

---

### peek vs get

```cpp
LRUCache<int, std::string> lru(3);
lru.put(1, "First");
lru.put(2, "Second");
lru.put(3, "Third");

// order: 3 (most recent) → 2 → 1 (least recent)

lru.peek(1); // reads "First" — order unchanged
// order still: 3 → 2 → 1

lru.get(1);  // reads "First" — promotes key 1 to front
// order now:  1 → 3 → 2

std::cout << *lru.mostRecentKey()  << "\n"; // 1
std::cout << *lru.leastRecentKey() << "\n"; // 2
```

---

### Eviction Policy

```cpp
LRUCache<int, std::string> lru(3);
lru.put(1, "One");
lru.put(2, "Two");
lru.put(3, "Three");

// order: 3 → 2 → 1 (key 1 is LRU)

lru.get(1); // promotes key 1 — order: 1 → 3 → 2
lru.get(3); // promotes key 3 — order: 3 → 1 → 2

// key 2 is now LRU
lru.put(4, "Four"); // evicts key 2

std::cout << lru.contains(2) << "\n"; // 0 — evicted
std::cout << lru.contains(4) << "\n"; // 1
std::cout << *lru.leastRecentKey() << "\n"; // 1
```

---

### Memoization

```cpp
LRUCache<int, long long> lru(10);

std::function<long long(int)> factorial = [&](int n) -> long long {
    if (n <= 1) return 1;

    auto* cached = lru.get(n);
    if (cached) return *cached;

    long long result = n * factorial(n - 1);
    lru.put(n, result);
    return result;
};

factorial(7); // computes 7! and caches intermediate results
factorial(5); // fully cached — no recomputation
```

---

### Cache Statistics

```cpp
LRUCache<int, std::string> lru(5);
lru.put(1, "A");
lru.put(2, "B");

lru.get(1); // hit
lru.get(2); // hit
lru.get(9); // miss
lru.get(8); // miss

std::cout << lru.hitCount()  << "\n"; // 2
std::cout << lru.missCount() << "\n"; // 2
std::cout << lru.hitRate()   << "\n"; // 50.0

lru.resetStats();
std::cout << lru.hitRate() << "\n"; // 0.0
```

---

### Move Ownership

```cpp
LRUCache<int, std::string> original(5);
original.put(1, "One");
original.put(2, "Two");

LRUCache<int, std::string> moved(std::move(original));

std::cout << original.size() << "\n"; // 0 — transferred
std::cout << moved.size()    << "\n"; // 2

auto* v = moved.get(1);
std::cout << *v << "\n"; // One
```

---

## Real-World Use Cases

| Use Case          | Key type          | Value type         | Notes                                   |
|-------------------|-------------------|--------------------|-----------------------------------------|
| DNS resolver      | `std::string`     | `std::string`      | Hostname → IP; evict stale entries      |
| Session store     | `std::string`     | `UserData`         | Token → user; evict inactive sessions  |
| Page cache        | `uint64_t`        | `Page`             | Page ID → page data; evict cold pages  |
| Memoization       | `Args` (tuple)    | `Result`           | Function call → cached result          |
| Image thumbnail   | `std::string`     | `Image`            | Path → decoded image; evict large items|
| API response cache| `std::string`     | `JsonResponse`     | URL → response; cap memory usage       |

---

## Core API

### Constructor & Destructor

```cpp
explicit LRUCache(std::size_t capacity);
// Throws std::invalid_argument if capacity == 0

~LRUCache();

// Copy is deleted — ownership is unambiguous via move
LRUCache(const LRUCache&)            = delete;
LRUCache& operator=(const LRUCache&) = delete;

LRUCache(LRUCache&& other) noexcept;
LRUCache& operator=(LRUCache&& other) noexcept;
```

---

### Cache Modifiers

```cpp
// Insert or update. Promotes to most-recent on update.
// Evicts LRU entry if at capacity and key is new.
void put(const K& key, const V& value);  // copy both
void put(const K& key, V&& value);       // copy key, move value
void put(K&& key, V&& value);            // move both

// Remove by key. Returns true if found and removed.
[[nodiscard]] bool erase(const K& key);

// Remove all entries. Capacity and sentinel nodes retained.
void clear();
```

---

### Cache Lookup

```cpp
// Lookup with LRU promotion. Increments hit/miss counter.
// Returns pointer to value, or nullptr on miss.
[[nodiscard]] V* get(const K& key);

// Read-only lookup. No LRU promotion. No stat tracking.
// Returns const pointer to value, or nullptr on miss.
[[nodiscard]] const V* peek(const K& key) const;

// Key existence check. No side effects.
[[nodiscard]] bool contains(const K& key) const;
```

---

### Capacity Management

```cpp
// Increase capacity limit. Throws if:
//   newCapacity == 0         → std::invalid_argument
//   newCapacity <= size()    → std::length_error
void resize(std::size_t newCapacity);
```

---

### Element Access

```cpp
// All keys in most-recent-first order.
[[nodiscard]] std::vector<K> keys() const;

// Pointer to the most recently accessed key. nullptr if empty.
[[nodiscard]] const K* mostRecentKey() const;

// Pointer to the least recently accessed key (evicted next). nullptr if empty.
[[nodiscard]] const K* leastRecentKey() const;
```

---

### Cache Statistics

```cpp
[[nodiscard]] std::size_t hitCount()  const noexcept; // get() hits
[[nodiscard]] std::size_t missCount() const noexcept; // get() misses
[[nodiscard]] double      hitRate()   const noexcept; // (hits/total)*100.0; 0.0 if no accesses

void resetStats() noexcept; // zero out hit and miss counters
```

---

### Capacity & State

```cpp
[[nodiscard]] std::size_t capacity() const noexcept; // max entries
[[nodiscard]] std::size_t size()     const noexcept; // current entries
[[nodiscard]] bool        empty()    const noexcept;
```

---

## Benchmark Results

Benchmarks compare LRUCache against `std::unordered_map` across 100K–800K operations. Times in microseconds (µs). Compiled without `-O2`.

> **Important context:** `std::unordered_map` is a plain hash map with no ordering overhead. LRUCache maintains a full doubly-linked list on every `get()` and `put()` — that is the cost being measured here. The comparison shows what LRU ordering actually costs, not a flaw in the implementation.

---

### Put — No Reserve

| Count   | LRUCache (µs) | unordered_map (µs) | Overhead |
|---------|-------------:|-------------------:|:--------:|
| 100,000 | 34,513       | 17,475             | ~2.0×    |
| 200,000 | 87,897       | 40,016             | ~2.2×    |
| 400,000 | 164,305      | 84,096             | ~2.0×    |
| 800,000 | 349,447      | 170,318            | ~2.1×    |

The ~2× overhead reflects the extra `new Node`, `insertFront`, and map pointer write that `put()` performs on top of a plain hash map insert.

---

### Put — With Reserve

| Count   | LRUCache (µs) | unordered_map (µs) | Overhead |
|---------|-------------:|-------------------:|:--------:|
| 100,000 | 42,234       | 20,196             | ~2.1×    |
| 200,000 | 111,829      | 39,166             | ~2.9×    |
| 400,000 | 210,557      | 54,274             | ~3.9×    |
| 800,000 | 399,928      | 91,703             | ~4.4×    |

Reserving buckets in `std::unordered_map` eliminates its rehashing cost, widening the gap. LRUCache has no equivalent `reserve()` exposed publicly — the linked list dominates its cost regardless of bucket pre-allocation.

---

### Get — Hit

| Count   | LRUCache (µs) | unordered_map (µs) | Overhead |
|---------|-------------:|-------------------:|:--------:|
| 100,000 | 26,349       | 11,641             | ~2.3×    |
| 200,000 | 48,641       | 21,699             | ~2.2×    |
| 400,000 | 92,896       | 41,167             | ~2.3×    |
| 800,000 | 157,268      | 78,590             | ~2.0×    |

Every `get()` hit calls `moveToFront()` — two pointer rewires in the doubly-linked list. That is the consistent ~2× overhead vs a plain map `find()`.

---

### Get — Miss

| Count   | LRUCache (µs) | unordered_map (µs) | Notes                            |
|---------|-------------:|-------------------:|----------------------------------|
| 100,000 | 20,925       | 0                  | map miss is essentially free     |
| 200,000 | 38,261       | 0                  |                                  |
| 400,000 | 78,951       | 0                  |                                  |
| 800,000 | 132,391      | 0                  |                                  |

`std::unordered_map::find()` on a miss returns `end()` almost instantly — the key simply isn't found and the function returns. LRUCache's `get()` still has to perform the hash map lookup and increment `missCounter`, which shows measurable cost at scale. The `0 us` values for the map reflect sub-microsecond timing below measurement resolution.

---

### Peek

| Count   | LRUCache (µs) | unordered_map (µs) | Notes       |
|---------|-------------:|-------------------:|:------------|
| 100,000 | 8,597        | 10,401             | ~1.2× faster|
| 200,000 | 21,360       | 20,854             | ~tie        |
| 400,000 | 42,261       | 41,926             | ~tie        |
| 800,000 | 84,858       | 85,933             | ~tie        |

`peek()` is LRUCache's purest benchmark — hash map lookup only, no list modification. It matches `std::unordered_map::find()` almost exactly, confirming the linked list is the source of overhead in all other operations.

---

### Erase

| Count   | LRUCache (µs) | unordered_map (µs) | Overhead |
|---------|-------------:|-------------------:|:--------:|
| 100,000 | 52,592       | 33,173             | ~1.6×    |
| 200,000 | 99,983       | 64,096             | ~1.6×    |
| 400,000 | 228,136      | 132,195            | ~1.7×    |
| 800,000 | 504,542      | 399,443            | ~1.3×    |

`erase()` calls `removeNode()` (pointer rewire) + `cache.erase()` + `delete node`. The map erase and node deletion dominate at scale; the list operation is small.

---

### Eviction

Fixed capacity of 1,000 — every insert forces an eviction.

| Count   | LRUCache (µs) | unordered_map (µs) | Overhead |
|---------|-------------:|-------------------:|:--------:|
| 100,000 | 218,595      | 51,408             | ~4.3×    |
| 200,000 | 181,712      | 92,896             | ~2.0×    |
| 400,000 | 473,500      | 163,542            | ~2.9×    |
| 800,000 | 655,493      | 320,238            | ~2.0×    |

This is the most expensive benchmark for LRUCache. Every `put()` triggers: `popBack()` + `cache.erase(lru_key)` + `delete lru` + `new Node` + `insertFront()` + `cache[key] = node`. The `std::unordered_map` baseline uses `map.erase(map.begin())` for simulated eviction, which is also O(1) but without node allocation overhead.

---

### Contains

| Count   | LRUCache (µs) | unordered_map (µs) | Notes  |
|---------|-------------:|-------------------:|:-------|
| 100,000 | 22,083       | 21,441             | ~tie   |
| 200,000 | 48,426       | 47,939             | ~tie   |
| 400,000 | 96,778       | 96,512             | ~tie   |
| 800,000 | 161,854      | 161,750            | ~tie   |

`contains()` delegates directly to `cache.contains()` — identical to `std::unordered_map::contains()`. The results confirm this: effectively zero overhead from the LRU layer.

---

### Mixed Workload

Interleaved `put()` and `get()` under capacity pressure (capacity = 1,000).

| Count   | LRUCache (µs) | unordered_map (µs) | Overhead |
|---------|-------------:|-------------------:|:--------:|
| 100,000 | 97,203       | 29,186             | ~3.3×    |
| 200,000 | 194,012      | 51,956             | ~3.7×    |
| 400,000 | 339,508      | 97,078             | ~3.5×    |
| 800,000 | 691,097      | 193,159            | ~3.6×    |

The most realistic benchmark. The ~3.5× overhead under mixed load is the combined cost of `moveToFront()` on every get and `popBack()` + node allocation/deallocation on every evicting put. This is the price paid for O(1) LRU eviction.

---

### Summary

| Operation          | Winner          | Notes                                                    |
|--------------------|-----------------|----------------------------------------------------------|
| Put (no reserve)   | unordered_map   | ~2× faster — no linked list overhead                    |
| Put (with reserve) | unordered_map   | Gap widens to ~4× — reserve eliminates map rehashing    |
| Get hit            | unordered_map   | ~2× faster — `moveToFront()` is LRUCache's core cost    |
| Get miss           | unordered_map   | Map miss is near-zero; LRUCache still traverses          |
| Peek               | **Tie**         | LRUCache matches map — no list modification              |
| Erase              | unordered_map   | ~1.5× faster — LRUCache adds node deletion              |
| Eviction           | unordered_map   | ~2–4× faster — alloc + dealloc per eviction             |
| Contains           | **Tie**         | Direct delegation to `cache.contains()`                  |
| Mixed workload     | unordered_map   | ~3.5× faster — overhead compounds under realistic load   |

> `std::unordered_map` wins every benchmark because it has no ordering overhead. LRUCache pays for O(1) LRU eviction with linked list pointer operations on every access. `peek()` and `contains()` are the exceptions — they bypass the list entirely and match the map's speed. This is the fundamental tradeoff of any LRU cache implementation.

---

## Project Structure

```
LRUCache/
├── include/
│   ├── LRUCache.h         # Class declaration, template parameters
│   ├── LRUCache.tpp       # All method definitions
│   └── Node.h             # Node<K, V> struct (key, value, prev*, next*)
│
├── benchmarks/
│   ├── benchmarks.cpp     # 9 benchmark suites vs std::unordered_map
│   └── utils/
│       ├── Table.h        # Benchmark result table formatting
│       └── Table.tpp
│
├── tests/
│   └── test.cpp           # Unit tests: lifecycle, operations, statistics, state
│
├── examples/
│   └── examples.cpp       # 9 real-world usage examples
│
├── README.md
└── LICENSE
```

---

## Build Instructions

### Requirements

- C++23-compatible compiler: GCC 13+, Clang 17+, or MSVC 19.38+
- No external dependencies — header-only core library

### Compile & Run Tests

```bash
g++ -std=c++23 tests/test.cpp -Iinclude -o build/tests
./build/tests
```

### Compile & Run Examples

```bash
g++ -std=c++23 examples/examples.cpp -Iinclude -o build/examples
./build/examples
```

### Compile & Run Benchmarks

```bash
g++ -std=c++23 benchmarks/benchmarks.cpp -Iinclude -Ibenchmarks/utils -o build/benchmarks
./build/benchmarks
```

> Use `-O2` or `-O3` for production-representative benchmark results.

---

## Notes

- **Not production-ready.** This is an educational project.
- Copy construction and copy assignment are intentionally deleted. The raw pointer ownership model makes deep copy non-trivial and was omitted by design. If you need a copyable cache, serialize via `keys()` and reconstruct.
- `get()` is the only operation that increments statistics. `peek()`, `contains()`, and `keys()` have no effect on hit/miss counters.
- `resize()` only increases capacity — it does not shrink. Calling `resize(n)` where `n <= size()` throws `std::length_error`.
- `put()` on an existing key updates the value in-place and promotes the node. It does **not** evict anything.
- `clear()` retains the sentinel nodes and capacity. The cache is ready to use immediately after `clear()`.
- `mostRecentKey()` and `leastRecentKey()` return `nullptr` when the cache is empty — always null-check before dereferencing.
- The linked list traversal in `keys()` returns keys in most-recent-first order, which matches the visual left-to-right order of the internal list.

---

## Contributing

Learning-focused PRs and improvements are welcome. Some areas worth exploring:

- Copy construction via `keys()` + re-insertion
- `getOrInsert(key, factory)` — get if present, else call factory and insert
- TTL (time-to-live) expiration per entry
- Thread-safe variant with `std::shared_mutex` (read-write lock)
- LFU (Least Frequently Used) variant as a comparison
- CMake build system
- CI pipeline (GitHub Actions)

---

## License

[MIT](LICENSE) — free to use, modify, and distribute for educational and personal purposes.
