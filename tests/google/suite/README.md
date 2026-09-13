# Google Test Suite

This document describes the test categories under `suite/` — what each
one verifies, and the individual test files it contains.

| Category | Focus |
|---|---|
| [Concurrency](#concurrency) | Thread-safety under concurrent reads and writes |
| [Integration](#integration) | Multiple components working together end-to-end |
| [Lifecycle](#lifecycle) | Construction, destruction, and moving |
| [Regression](#regression) | Previously fixed bugs that must stay fixed |
| [Unit](#unit) | Individual functions or methods in isolation |

Unlike the benchmark suite, tests validate the library's own correctness
directly — there is no reference implementation to compare against, so
results are simply pass or fail.

Every test case is a Google Test `TEST(Suite, Name)`, which registers
itself automatically at startup — there's no suite list, and no sequential
id, to maintain by hand. A single file may contain several `TEST()` cases
sharing the file's `Suite` name.

---

## Concurrency

Verifies thread-safety — concurrent reads and writes from multiple
threads, and correctness under simultaneous access.

### Tests

| File | What it covers |
|---|---|
| `external_locking_contract.cpp` | Correct behavior when access is serialized by an external mutex (put/get/erase correctness, visibility across threads) |
| `external_synchronization.cpp` | Concurrent callers stay safe under an external mutex, tracked via atomic completion counters |

---

## Integration

Verifies multiple components working together end-to-end — for example,
a full parse-modify-serialize round trip — rather than a single function
in isolation.

### Tests

| File | What it covers |
|---|---|
| `eviction_order.cpp` | Fill to capacity, insert past it, correct LRU entry evicted end-to-end |
| `pool_reuse.cpp` | Evict/insert and erase/insert cycles actually recycle freed slots rather than growing storage |
| `resize_workflow.cpp` | Grow then shrink then continue normal use |
| `stats_tracking.cpp` | Hit/miss counters stay correct across a realistic mixed sequence |
| `put_get_roundtrip.cpp` | Basic full-cycle sanity check |

---

## Lifecycle

Verifies object lifetime operations — construction, destruction, and
moving.

### Tests

| File | What it covers |
|---|---|
| `construction.cpp` | Ctor behavior across valid/zero/edge capacities, plus pool and table sized for full capacity |
| `destruction.cpp` | Clean teardown, no leaks, values with non-trivial destructors |
| `move_semantics.cpp` | Move construction and move assignment: state transfer, self-move, moved-from validity, empty-source branch |

---

## Regression

Verifies that a specific, previously fixed bug stays fixed. One test per
resolved issue, added at the time the fix lands.

### Tests

| File | What it covers |
|---|---|
| `resize_alloc_failure.cpp` | resize() use-after-free when table allocation throws mid-resize |
| `throwing_ctor_recovery.cpp` | acquireNode()/acquireNodeEmplace() restore the free list correctly when K's or V's constructor throws |
| `update_skips_eviction.cpp` | put()/emplace() must not evict when the key already exists at full capacity (update path only) |
| `erase_slot_wraparound.cpp` | Backward-shift deletion stays correct when the probe cluster wraps past the end of the table |
| `already_front_noop.cpp` | moveToFront()'s early-return branch (node already MRU) doesn't corrupt list links |
| `empty_source_move.cpp` | Move ctor/assign from an already-empty cache takes the empty-list branch correctly |
| `full_load_guard.cpp` | tableSizeFor()'s guaranteed empty slot prevents an infinite loop in findSlot()/insertSlot() at max load |
| `moved_from_put.cpp` | put() on a moved-from (capacity 0) cache must fail safely rather than dereference a null table |

---

## Unit

Verifies individual functions or methods in isolation — the smallest
testable unit of behavior, independent of the categories above.

### Tests

| File | What it covers |
|---|---|
| `put.cpp` | All put() overloads: copy/move key, copy/move value, insert vs update path |
| `get.cpp` | Hit and miss paths; verifies recency order updates on hit |
| `peek.cpp` | Hit and miss paths; verifies recency order is untouched |
| `contains.cpp` | Existing key vs missing key |
| `erase.cpp` | Existing key, missing key, post-erase state |
| `emplace.cpp` | Insert path, update path, no intermediate value temporary |
| `clear.cpp` | Cache emptied correctly, reusable afterward |
| `resize.cpp` | Grow, shrink, shrink below live entry count (forces eviction) |
| `reserve.cpp` | Confirms documented no-op contract |
| `keys.cpp` | Recency-ordered key traversal |
| `observer.cpp` | size(), empty(), capacity(), hitCount(), missCount(), hitRate(), mostRecentKey(), leastRecentKey() |
| `shrink_to_fit.cpp` | Reclaims unused capacity down to size(); no-op when empty or already full |

---

## Convention

Rules every file in this suite follows, so new tests stay consistent with
the existing ones:

- **Includes.** `<gtest/gtest.h>` first, then `<CachePro/LRUCache.h>`, then
  standard library headers — no `support/` header, since tests validate
  the library directly rather than comparing against a reference.
- **Namespaces.** `using namespace CachePro;` at file scope — no
  per-call qualification.
- **Naming.** `TEST(Suite, CaseName)`, where `Suite` is the `PascalCase`
  form of the file's subject (`Put`, `EraseSlotWraparound`) and
  `CaseName` describes the specific scenario (`InsertPathBecomesMru`,
  `MissingKeyReturnsFalse`) — no `test_` prefixes or underscores.
- **Assertions.** `EXPECT_*` for checks that shouldn't stop the test on
  failure (the normal case); `ASSERT_*` only where continuing would
  dereference something that might not exist — most commonly a pointer
  returned by `peek()`/`get()`/`mostRecentKey()`/`leastRecentKey()`.
  Every such pointer gets `ASSERT_NE(ptr, nullptr)` before it's
  dereferenced, even where the original assumed success.
- **Exceptions.** `ASSERT_THROW(expr, ExceptionType)` for any call
  expected to throw; wrap the expression in parentheses if it contains a
  comma (e.g. template arguments).
- **Equality on unsigned results.** `size()`, `capacity()`, `hitCount()`,
  and `missCount()` return unsigned types — compare against unsigned
  literals (`3u`, `0u`) to avoid signed/unsigned comparison warnings.
- **Floating point.** `hitRate()` comparisons use `EXPECT_DOUBLE_EQ`, not
  `EXPECT_EQ`.
- **No shared main.** No file defines `main()`; that lives in a single
  runner (or `gtest_main`) shared by the whole suite.
