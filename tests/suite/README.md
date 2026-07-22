# Test Suite

This document describes the test categories under `suite/` — what each
one verifies, and the individual test files it contains.

Unlike the benchmark suite, tests validate the library's own
correctness directly — there is no reference implementation to compare
against, so results are simply pass or fail.

Every test suite registers itself automatically via
`REGISTER_TEST_SUITE()` at startup, and is assigned a sequential id
within its category (e.g. `U1`, `U2` for Unit; `L1`, `L2` for
Lifecycle) — there's no suite list to maintain by hand. This applies
uniformly across every category below.

---

## Concurrency

Verifies thread-safety — concurrent reads and writes from multiple
threads, and correctness under simultaneous access.

### Tests

- `external_locking_contract.cpp` — Correct behavior when access is serialized by an external mutex (put/get/erase correctness, visibility across threads)
- `external_synchronization.cpp` — Concurrent callers stay safe under an external mutex, tracked via atomic completion counters

---

## Integration

Verifies multiple components working together end-to-end — for
example, a full parse-modify-serialize round trip — rather than a
single function in isolation.

### Tests

- `eviction_order.cpp` — Fill to capacity, insert past it, correct LRU entry evicted end-to-end
- `pool_reuse.cpp` — Evict/insert and erase/insert cycles actually recycle freed slots rather than growing storage
- `resize_workflow.cpp` — Grow then shrink then continue normal use
- `stats_tracking.cpp` — Hit/miss counters stay correct across a realistic mixed sequence
- `put_get_roundtrip.cpp` — Basic full-cycle sanity check

---

## Lifecycle

Verifies object lifetime operations — construction, destruction,
copying, and moving — across the different value kinds a JSON value
can hold (null, bool, number, string, array, object).

### Tests

- `construction.cpp` — Ctor behavior across valid/zero/edge capacities, plus pool and table sized for full capacity
- `destruction.cpp` — Clean teardown, no leaks, values with non-trivial destructors
- `move_semantics.cpp` — Move construction and move assignment: state transfer, self-move, moved-from validity, empty-source branch

---

## Regression

Verifies that a specific, previously fixed bug stays fixed. One test
per resolved issue, added at the time the fix lands.

### Tests

- `resize_alloc_failure.cpp` — resize() use-after-free when table allocation throws mid-resize
- `throwing_ctor_recovery.cpp` — acquireNode()/acquireNodeEmplace() restore the free list correctly when K's or V's constructor throws
- `update_skips_eviction.cpp` — put()/emplace() must not evict when the key already exists at full capacity (update path only)
- `erase_slot_wraparound.cpp` — Backward-shift deletion stays correct when the probe cluster wraps past the end of the table
- `already_front_noop.cpp` — moveToFront()'s early-return branch (node already MRU) doesn't corrupt list links
- `empty_source_move.cpp` — Move ctor/assign from an already-empty cache takes the empty-list branch correctly
- `full_load_guard.cpp` — tableSizeFor()'s guaranteed empty slot prevents an infinite loop in findSlot()/insertSlot() at max load
- `moved_from_put.cpp` — put() on a moved-from (capacity 0) cache must fail safely rather than dereference a null table

---

## Unit

Verifies individual functions or methods in isolation — the smallest
testable unit of behavior, independent of the categories above.

### Tests

- `put.cpp` — All put() overloads: copy/move key, copy/move value, insert vs update path
- `get.cpp` — Hit and miss paths; verifies recency order updates on hit
- `peek.cpp` — Hit and miss paths; verifies recency order is untouched
- `contains.cpp` — Existing key vs missing key
- `erase.cpp` — Existing key, missing key, post-erase state
- `emplace.cpp` — Insert path, update path, no intermediate value temporary
- `clear.cpp` — Cache emptied correctly, reusable afterward
- `resize.cpp` — Grow, shrink, shrink below live entry count (forces eviction)
- `reserve.cpp` — Confirms documented no-op contract
- `keys.cpp` — Recency-ordered key traversal
- `observers.cpp` — size(), empty(), capacity(), hitCount(), missCount(), hitRate(), mostRecentKey(), leastRecentKey()
- `shrink_to_fit.cpp` — Reclaims unused capacity down to size(); no-op when empty or already full
