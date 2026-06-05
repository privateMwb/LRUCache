# LRUCache

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

This project is primarily educational and is not intended for production use.

It was built to deeply understand:

- Cache eviction (LRU strategy)
- Hash map + linked list integration
- Modern C++ design patterns
- Move semantics and resource handling

## License

MIT License