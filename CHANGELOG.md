# Changelog

All notable changes to LRUCache are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Nothing yet.

## [1.0.0] - 2026-07-31

The first stable release of LRUCache, a fixed-capacity, header-only LRU
cache for modern C++23.

### Added
- `put()` and `emplace()` for insertion, with an insert-or-update
  contract: an existing key is updated and moved to most-recently-used
  in place, without evicting anything else.
- `get()`, `peek()`, and `contains()` lookups — `get()` marks a hit as
  most-recently-used and counts toward `hitCount()`/`missCount()`;
  `peek()` and `contains()` never affect recency or the counters.
- `erase()` and `clear()` for removal.
- `resize()`, `reserve()`, and `shrink_to_fit()` for explicit capacity
  management: `resize()` grows or shrinks (evicting least-recently-used
  entries first if needed), `reserve()` is a documented no-op, and
  `shrink_to_fit()` reclaims unused pool/table space down to the
  current size.
- Move construction and move assignment, including safe
  self-move-assignment. A moved-from cache is left empty
  (`size() == 0`, `capacity() == 0`) and safe to destroy or
  move-assign into again.
- `hitCount()`, `missCount()`, `hitRate()`, and `resetStats()` for
  built-in usage statistics.
- `keys()`, `mostRecentKey()`, and `leastRecentKey()` for
  recency-ordered introspection.
- Custom `Hash` and `KeyEqual` template parameters, defaulting to
  `std::hash<K>` and `std::equal_to<K>`.
- `rain::` namespace alias for `LRUCache`.

### Performance
- Pool-allocated intrusive doubly linked list: every node lives in one
  array allocated once at construction, reused via an embedded free
  list on eviction/erase, so steady-state operation performs zero heap
  allocations.
- Flat open-addressing table (linear probing, backward-shift deletion)
  for key-to-node lookup instead of `std::unordered_map`'s chaining —
  bounded load factor (≤ 0.5) and no tombstone accumulation, so probe
  sequences never degrade over the cache's lifetime.
- Slot-caching nodes: every node remembers its own table index, so
  eviction never re-hashes or re-probes to find its table entry.
- `resize()`'s lazy path pads physical reservation by 1.5× on real
  growth, so a run of small incremental `resize()` calls hits the
  no-reallocation path instead of reallocating on every call.
- Strong exception guarantee on insert: a node is fully constructed
  before it's linked into the table or list, so a throwing `K`/`V`
  constructor leaves the cache unchanged and its pool slot is restored
  to the free list rather than leaked.
- `resize()` allocates the new table before touching existing state, so
  a failed allocation (`std::bad_alloc`) leaves the cache exactly as it
  was, with no use-after-free on the next call.
- Benchmarked against `stdLRU` (`std::list` + `std::unordered_map`) at
  10K / 100K / 1M iterations; largest wins on miss-heavy workloads
  (`contains()`/`get()` misses), bulk churn (`clear()` + refill), and
  `reserve()` — a genuine no-op here versus a real rehash in `stdLRU`.
  Trade-off: the pool and table are allocated eagerly at construction,
  so `Construct Empty` and reallocation-triggering calls (`resize()`
  growth, `shrink_to_fit()`) are slower than `stdLRU`'s lazy
  allocation. Full results in `benchmarks/results/v1_0_0.md`.

### Testing
- Comprehensive test suite covering unit, integration, lifecycle,
  regression, and concurrency tests; move semantics; exception safety
  and allocation-failure handling; resize behavior; hash-table edge
  cases (including probe-cluster wraparound); cache eviction order;
  pool slot reuse; statistics tracking; and external synchronization
  contracts.
- 99.0% line coverage and 96.2% function coverage, excluding test
  infrastructure.

### CI
- Automated builds and tests across GCC, Clang, MSVC, and AppleClang,
  each in Debug and Release configurations.

[Unreleased]: https://github.com/privateMwb/LRUCache/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/privateMwb/LRUCache/releases/tag/v1.0.0
