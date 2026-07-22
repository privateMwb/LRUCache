# Benchmark Suite

This document describes the benchmark categories under `suite/` — what
each one measures, and the individual benchmarks it contains.

Every benchmark compares LRUCache against stdLRU — a std::list +
std::unordered_map implementation, the conventional way an LRU cache is
built in C++. A category can support more than one standard for
comparison, but for now each category is benchmarked against a single
standard.

Every `BENCH()` call, in every category below, is automatically repeated
at three iteration tiers — SMALL (10K), MEDIUM (100K), and LARGE (1M) —
to smooth out timing noise and show whether relative performance holds
steady as call volume increases. This applies uniformly across the whole
suite; it is not specific to any one category. The **Scaling** category
below measures something different: how per-operation cost changes as
capacity itself grows or shrinks, independent of iteration count.

Some benchmarks have no meaningful stdLRU equivalent — stdLRU tracks no
hit/miss statistics and has no recency-order introspection. Those run
through `BENCH_SOLO()` instead of `BENCH()`, timing LRUCache alone.

---

## Access

Benchmarks read and lookup operations on an already-full cache —
retrieving values by key, recency-ordered traversal, and existence
checks.

### Benchmarks

- `element_access.cpp` — `get()` hit (recency-updating), `peek()` hit
  (recency-neutral)
- `iteration.cpp` — `keys()` full recency-ordered traversal
- `search.cpp` — `contains()` miss, `get()` miss

---

## Core

Benchmarks the fundamental, most frequently exercised operations —
inserting, evicting, erasing, and clearing entries.

### Benchmarks

- `insert.cpp` — `put()` insert, cache below capacity (no eviction)
- `push_back.cpp` — `put()` insert at capacity (steady-state eviction on
  every call)
- `emplace.cpp` — `emplace()` insert below capacity, no intermediate
  value temporary
- `erase.cpp` — `erase()` on an existing key
- `pop_clear.cpp` — `clear()` on a full cache, then refill from empty

---

## Lifecycle

Benchmarks object lifetime operations — construction, destruction, and
moving. LRUCache has no copy constructor, so this category covers move
only.

### Benchmarks

- `construction.cpp` — constructing an empty cache sized for N entries
- `move.cpp` — move-assign (ping-ponged between two populated caches),
  move-construct

---

## Scaling

Benchmarks how per-operation cost changes as capacity itself grows or
shrinks — a separate axis from the SMALL/MEDIUM/LARGE iteration tiers
described above: those repeat the same fixed-size operation more times,
while Scaling grows or shrinks the cache itself and observes the
resulting cost.

### Benchmarks

- `reallocation.cpp` — `resize()` growing capacity by one entry, repeated
- `reserve.cpp` — `reserve()` (documented no-op) against stdLRU's real
  `unordered_map::reserve()`
- `shrink_to_fit.cpp` — `shrink_to_fit()` reclaiming slack added on a
  cycling growth pattern

---

## Utility

Benchmarks introspection and bookkeeping operations that don't belong
to any of the categories above — hit/miss statistics, recency-order
lookups, and current-state reporting.

### Benchmarks

- `observer.cpp` — `hitCount()`, `missCount()`, `hitRate()`,
  `mostRecentKey()`, `leastRecentKey()` (solo, no stdLRU equivalent)
- `state.cpp` — `size()`, `empty()`, `capacity()` (paired against stdLRU)