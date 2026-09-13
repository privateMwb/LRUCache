# Benchmark Suite

This document describes the benchmark categories under `suite/` — what each
one measures, and the individual benchmarks it contains.

| Category | Focus |
|---|---|
| [Access](#access) | Reads, lookups, and traversal on an already-populated cache |
| [Core](#core) | Inserting, emplacing, erasing, and clearing entries |
| [Lifecycle](#lifecycle) | Construction and moving |
| [Scaling](#scaling) | Cost vs. capacity growth/shrink, independent of iteration count |
| [Utility](#utility) | Bookkeeping and state accessors |

Every benchmark compares CachePro's `LRUCache` against `stdLRU` — a
hand-rolled `std::list` + `std::unordered_map` reference implementation, and
the natural baseline for what an LRU cache built from standard containers
would look like. A category can support more than one standard for
comparison, but for now each category is benchmarked against a single
standard.

Every benchmark is a Google Benchmark `BENCHMARK()` case: Google Benchmark
runs each one for as many iterations as it takes to reach a stable
measurement, rather than the fixed SMALL/MEDIUM/LARGE call tiers an older
in-house framework used. The **Scaling** category below measures something
different: how per-operation cost changes as capacity itself grows or
shrinks, independent of iteration count.

Some benchmarks have no meaningful stdLRU equivalent — stdLRU tracks none
of LRUCache's hit/miss bookkeeping or recency-key introspection. Those are
plain `LRUCache`-only functions with no `_stdLRU` counterpart, instead of
the usual paired `_LRUCache` / `_stdLRU` pair.

---

## Access

Benchmarks read and lookup operations on an already-populated cache —
element access, traversal, and lookup misses.

### Benchmarks

| File | What it covers |
|---|---|
| `element_access.cpp` | `get()` hit and `peek()` hit |
| `iteration.cpp` | `keys()` — full recency-ordered traversal |
| `search.cpp` | `contains()` miss, and `get()` miss |

---

## Core

Benchmarks the fundamental, most frequently exercised operations —
inserting, emplacing, erasing, and clearing entries.

### Benchmarks

| File | What it covers |
|---|---|
| `insert.cpp` | `put()` inserting a brand-new key, cache below capacity |
| `push_back.cpp` | `put()` inserting a brand-new key, cache already at capacity (evicting) |
| `emplace.cpp` | `emplace()` constructing a brand-new value in place, cache below capacity |
| `erase.cpp` | `erase()` on an existing key |
| `pop_clear.cpp` | `clear()` on a full cache, then refill from empty |

---

## Lifecycle

Benchmarks object lifetime operations — construction and moving.

### Benchmarks

| File | What it covers |
|---|---|
| `construction.cpp` | Constructing (and destroying) an empty cache |
| `move.cpp` | Move-assign between two populated caches, and move-construct from a freshly populated source |

---

## Scaling

Benchmarks how per-operation cost changes as capacity itself grows or
shrinks — a separate axis from Google Benchmark's own iteration count:
that repeats the same fixed-size operation more times, while Scaling grows
or shrinks the cache itself and observes the resulting cost.

### Benchmarks

| File | What it covers |
|---|---|
| `reallocation.cpp` | `resize()` growing capacity by one entry, repeated |
| `reserve.cpp` | `reserve()` on an already-constructed cache (no-op on LRUCache, a real rehash on stdLRU) |
| `shrink_to_fit.cpp` | `shrink_to_fit()` after a bounded, cycling amount of over-reservation |

---

## Utility

Benchmarks bookkeeping and state-accessor operations that don't belong to
any of the categories above.

### Benchmarks

| File | What it covers |
|---|---|
| `observer.cpp` | `hitCount()`, `missCount()`, `hitRate()`, `mostRecentKey()`, `leastRecentKey()` (no stdLRU equivalent) |
| `state.cpp` | `size()`, `empty()`, and `capacity()` (paired against stdLRU) |
