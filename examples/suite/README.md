# Example Suite

This document describes the example categories under `suite/` — what
each one demonstrates, and the individual example files it contains.

Unlike the test suite, an example doesn't assert correctness — it
demonstrates real usage of the library, including deliberate misuse
where instructive (see Misuse), so the reader sees both the correct
pattern and the mistake it guards against.

Every example file ends with `REGISTER_EXAMPLE_SUITE()`, which derives
the suite's category from its containing directory and assigns it a
sequential id within that category. This applies uniformly across
every category below.

---

## Advanced

Demonstrates deeper mechanics of the library — move semantics,
exception safety, changing capacity after construction, and the
observable consequences of the pool-based node design.

### Examples

- `move_semantics.cpp` — move construction/assignment, and what's actually safe to call on a moved-from cache
- `exception_safety.cpp` — the strong exception guarantee on emplace() when a value's constructor throws
- `resize_and_reserve.cpp` — resize() growing and shrinking, reserve() as a documented no-op, shrink_to_fit()
- `node_internals.cpp` — a fixed footprint under heavy put() churn, and slot reuse after erase(), observed through the public API

---

## Integration

Demonstrates interoperability with the rest of a codebase — custom key
types, the standard algorithms library, exporting a snapshot to other
formats, and embedding the cache inside a larger class.

### Examples

- `custom_key_types.cpp` — a compound key type with a custom Hash functor
- `stl_algorithms.cpp` — keys() as a plain std::vector, used with std::find, std::ranges::sort, std::accumulate, and std::count_if
- `snapshot_export.cpp` — exporting cache contents to a std::map and to CSV lines via peek()
- `embedding_in_class.cpp` — wrapping LRUCache as a private implementation detail behind a domain-specific API

---

## Misuse

Demonstrates common mistakes and the exceptions or undefined behavior
they lead to, alongside the correct pattern — including examples shown
but not executed, so the reader can see what to avoid without the
program actually invoking undefined behavior.

### Examples

- `zero_capacity.cpp` — constructing with capacity 0 throws std::invalid_argument
- `resize_to_zero.cpp` — resize(0) throws the same way, leaving the cache untouched
- `use_after_move.cpp` — put() throws cleanly on a moved-from cache; why get()/peek()/contains() would not
- `stale_value_pointer.cpp` — a get()/peek() pointer left dangling by a reallocating resize() or shrink_to_fit()
- `null_key_pointer.cpp` — mostRecentKey()/leastRecentKey() returning nullptr on an empty cache

---

## Patterns

Demonstrates common usage idioms built on top of the core API —
cache-aside loading, read-only inspection, stats-driven capacity
tuning, and bulk seeding from an external source.

### Examples

- `cache_aside.cpp` — checking the cache first, falling back to a slower source on a miss, and caching the result
- `peek_without_promoting.cpp` — auditing every entry with peek() instead of get(), so recency order survives untouched
- `stats_driven_tuning.cpp` — using hitCount()/missCount()/hitRate() to recognize thrashing and resize() accordingly
- `bulk_seeding.cpp` — preloading a cache from an existing container, and the gotcha when seed data exceeds capacity

---

## Quickstart

Demonstrates fundamental, everyday usage — construction, inserting and
retrieving entries, checking capacity and size, and reading entries
without disturbing recency order.

### Examples

- `basic_usage.cpp` — construction, put()/get(), capacity(), size(), empty(), clear()
- `filling_a_cache.cpp` — filling to capacity, updating a key in place, the move-taking put() overload, eviction
- `inspecting_entries.cpp` — peek() vs get(), contains(), keys()
