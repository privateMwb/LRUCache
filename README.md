# LRUCache

![C++](https://img.shields.io/badge/C%2B%2B-23-blue)
![Status](https://img.shields.io/badge/status-learning-green)

---

## Overview

A custom **Least Recently Used (LRU) Cache** implementation written in modern C++.

This project was built to explore:

- Hash tables (`std::unordered_map`)
- Doubly linked lists
- Cache eviction strategies
- Move semantics
- Template programming
- Resource management
- Performance benchmarking

The cache provides **O(1)** average-time lookup, insertion, update, and eviction.

---

## Motivation / Goals

The goal of this project is to understand how a real-world caching system works internally by building it from scratch in modern C++.

Instead of relying on high-level abstractions, this implementation focuses on learning the underlying mechanics of:

- how hash tables provide fast key lookup
- how a doubly linked list maintains usage order efficiently
- how LRU (Least Recently Used) eviction policies are implemented in practice
- how memory ownership and pointer management work in performance-critical data structures
- how move semantics improve efficiency by avoiding unnecessary copies

This project also serves as a practical exercise in writing:

- clean template-based C++ code
- safe resource management logic
- predictable O(1) data structure operations
- benchmarkable and testable systems

Overall, the goal is not just to build a cache, but to deeply understand the design decisions behind high-performance in-memory storage systems.

---

## Features

- Generic key and value types
- Custom hash function support
- O(1) `get()`
- O(1) `put()`
- O(1) `erase()`
- Automatic LRU eviction
- Cache statistics
  - Hit count
  - Miss count
  - Hit rate
- Peek without affecting usage order
- Most recently used key inspection
- Least recently used key inspection
- Move constructor
- Move assignment operator
- Unit tests
- Benchmarks

---

## Design Overview

The LRU Cache is built using a **combination of a doubly linked list and a hash map** to achieve O(1) operations.

### Core Idea

- Hash Map → fast key lookup
- Doubly Linked List → maintain usage order (LRU → MRU)

---

### Internal Structure

```
          +----------------------+
          |   unordered_map      |
          |  key -> Node*        |
          +----------+-----------+
                     |
                     v
        +---------------------------+
        |     Doubly Linked List    |
        |                           |
        |  MRU  <->  ...  <->  LRU  |
        |   ^                 ^     |
        |   |                 |     |
        +---|-----------------|-----+
            head             tail
```

---

### Access Flow (get / put)

```
GET(key)
  |
  v
[Hash Map lookup]
  |
  +--> Not found → MISS
  |
  +--> Found
         |
         v
   Move node to FRONT (MRU)
         |
         v
       Return value


PUT(key, value)
  |
  v
[Key exists?]
  |
  +--> YES → update value + move to FRONT
  |
  +--> NO  → create node
               |
               v
        Insert at FRONT
               |
               v
        If size > capacity:
               remove LRU (TAIL)
```

---

### Eviction Policy

```
Least Recently Used (LRU) = tail node

When capacity is exceeded:
    remove tail
```

---

### Complexity Guarantee

All major operations run in:

- O(1) average time
- O(n) only during rehash (rare)

---

## Quick Example

```cpp
#include <iostream>
#include <string>

#include "LRUCache.h"

int main() {
    LRUCache<int, std::string> cache(3);

    cache.put(1, "Apple");
    cache.put(2, "Banana");
    cache.put(3, "Mango");

    if(auto* value = cache.get(2)) {
        std::cout << *value << '\n';
    }

    return 0;
}
```

---

## Core Operations

```cpp
cache.put(key, value);
cache.get(key);

cache.erase(key);
cache.clear();

cache.contain(key);
cache.peek(key);

cache.hitCount();
cache.missCount();
cache.hitRate();

cache.mostRecentKey();
cache.leastRecentKey();
```

---

## Complexity

| Operation | Average Complexity |
|----------|--------------------|
| `get()`  | O(1)               |
| `put()`  | O(1)               |
| `erase()`| O(1)               |
| `contain()` | O(1)            |
| `peek()`  | O(1)               |

> Note: Average-case assumes a well-distributed hash function and stable memory allocation behavior.

---

## Benchmarks

The project includes benchmarks for:

### Put / Get Benchmark

Measures cache insertion and retrieval performance across different cache capacities.

### Mixed Workload Benchmark

Simulates realistic cache usage patterns:

- 50% Put / 50% Get  
- 20% Put / 80% Get  
- 10% Put / 90% Get  
- 25% Put / 75% Get  
- 5% Put / 95% Get  

---

## Tests

The project includes unit tests covering:

- Basic insertion
- Retrieval
- LRU ordering
- Eviction behavior
- Key updates
- Erase operation
- Clear operation
- Containment checks
- Peek operation
- Cache statistics
- Reserve behavior
- Capacity state
- Move semantics

---

## Project Structure

```txt
LRUCache/
├── LRUCache.h
├── LRUCache.tpp
├── Node.h
│
├── tests/
│   └── test.cpp
│
├── benchmarks/
│   └── benchmark.cpp
│
├── examples/
│   └── examples.cpp
│
├── build/
│   ├── test
│   ├── benchmark
│   └── examples
│
├── README.md
└── LICENSE
```

---

## Build

### Compile Tests

```bash
g++ -std=c++20 tests/test.cpp -o test
./test
```

### Compile Benchmarks

```bash
g++ -std=c++20 benchmarks/benchmark.cpp -o benchmark
./benchmark
```

### Compile Examples

```bash
g++ -std=c++20 examples/examples.cpp -o examples
./examples
```

---

## Notes

This project is primarily educational and is intended to demonstrate how an LRU cache works internally rather than serving as a production-ready caching system.

### Limitations

- Not thread-safe (no concurrency control or locking mechanisms)
- No custom allocator support
- No advanced memory pooling optimizations
- Performance depends on the quality of the hash function and system memory behavior

### Optimization Notes

- Core operations (`get`, `put`, `erase`) are designed to run in **O(1)** average time
- Uses a combination of `std::unordered_map` and a doubly linked list to minimize overhead
- Move semantics are implemented to reduce unnecessary copying
- Cache efficiency may vary under extreme workloads or poor hash distribution

### Educational Purpose

This project was built to understand:

- LRU eviction strategy design
- Hash map + linked list integration
- Pointer-based data structure management
- Cache performance trade-offs
- Modern C++ design techniques

---

## License

MIT License