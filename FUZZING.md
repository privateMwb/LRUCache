# Fuzzing

LRUCachePro is fuzzed via [ClusterFuzzLite](https://google.github.io/clusterfuzzlite/),
running on every pull request that touches the fuzzed files, plus a
longer scheduled batch run every night.

## What's covered

**`fuzz_lrucache.cpp`** is a differential fuzzer: it runs the same
sequence of operations against `LRUCache<int, int>` and a
`std::list<std::pair<int,int>>` shadow model kept in the same
most-recently-used-first order the real cache is documented to
maintain, comparing size, capacity, contents, and recency order after
every single operation (not just at the end), so a failing input
localizes to the exact operation that broke an invariant.

`int` is used for both the key and value deliberately, not for
simplicity — it makes every `put()`/`emplace()`/`erase()` cheap enough
that a single fuzz run can drive thousands of insert/evict/erase
cycles, which is what actually stresses the two hand-written
subsystems `LRUCache` leans on instead of standard containers, rather
than the value type's own construction/copy logic.

Specifically exercised:

- **The pool's free list** (`acquireNode()`/`acquireNodeEmplace()`/
  `releaseNode()`) under repeated evict-then-insert and
  erase-then-insert cycles, biased toward small starting capacities so
  most runs force eviction almost immediately. `pool_reuse.cpp` and
  `full_load_guard.cpp` probe this by hand for a few chosen sequences;
  the fuzzer explores orderings a person wouldn't think to write.
- **The open-addressing table's linear probing and backward-shift
  deletion** (`findSlot()`/`insertSlot()`/`eraseSlot()`). Small
  capacities combined with `std::hash<int>`'s real distribution
  produce wraparound clusters "for free" across many different key
  sequences, rather than the one hand-constructed colliding-hash case
  in `erase_slot_wraparound.cpp`.
- **`get()` vs. `peek()` vs. `contains()`** — the harness confirms
  exactly one of the three ever moves an entry to most-recently-used or
  touches `hitCount()`/`missCount()`, on both the hit and miss path.
- **`resize()`'s full contract**: growing, shrinking to a size still
  covering every live entry, and shrinking below the live count (which
  must evict least-recently-used entries first). `resize(0)` is
  checked separately: it must throw `std::invalid_argument` and leave
  the cache byte-for-byte unchanged.
- **`reserve()`'s documented no-op contract**, and **`shrink_to_fit()`'s**
  reclaim-to-`size()` / no-op-when-empty-or-full contract.
- **Move construction and move assignment, including
  self-move-assignment.** These always move a freshly built scratch
  cache into the cache under test (or vice versa, then back), so the
  persistent object being fuzzed is never itself left in a moved-from
  state — see "What's deliberately NOT covered yet" below for why.

Built and run under both AddressSanitizer and UndefinedBehaviorSanitizer.

## What's deliberately NOT covered yet

- **Custom `Hash`/`KeyEqual` template parameters.** Only the defaults
  (`std::hash<int>`, `std::equal_to<int>`) are exercised. A harness
  using a deliberately colliding custom `Hash` (rather than relying on
  `std::hash<int>`'s natural distribution) would stress the
  open-addressing table's worst case far more aggressively — a
  natural follow-up harness, not a change to this one.
- **Exception injection during `put()`/`emplace()`.** `int` can't
  throw, so `acquireNode()`/`acquireNodeEmplace()`'s `catch (...)`
  free-list-restoration path (see `throwing_ctor_recovery.cpp`) is
  never exercised here. A throwing test value type (throws on the Nth
  construction) is what a follow-up harness would need to actually
  fuzz that rollback path across arbitrary operation sequences, rather
  than the few hand-picked ones in the regression suite.
- **`put()`/`get()`/etc. on an already-moved-from cache.** `moved_from_
  put.cpp` documents that this currently indexes into a null table —
  undefined behavior, not a clean failure. Deliberately excluded from
  this harness rather than fixed: including it here would mean nearly
  every run crashes on the same known, already-documented issue,
  drowning out anything else the fuzzer might find. Once that bug is
  fixed, exercising the moved-from state is the natural next thing to
  add.

## Running locally

```bash
git clone --recursive https://github.com/google/oss-fuzz.git
cd oss-fuzz
python infra/helper.py build_fuzzers --sanitizer address LRUCachePro /path/to/LRUCachePro
python infra/helper.py run_fuzzer LRUCachePro fuzz_lrucache
```

Or, without OSS-Fuzz's tooling, directly with clang:

```bash
clang++ -std=c++20 -fsanitize=fuzzer,address \
  -Iinclude \
  fuzz/fuzz_lrucache.cpp \
  -o fuzz_lrucache

./fuzz_lrucache
```

Add `-fsanitize=fuzzer,undefined` instead to run under UBSan.

## Reproducing a crash

ClusterFuzzLite uploads the failing input as a workflow artifact when
a run fails. Download it, then:

```bash
./fuzz_lrucache path/to/crash-<hash>
```

This replays that exact byte sequence through
`LLVMFuzzerTestOneInput()` once, deterministically — no sanitizer flags
needed beyond however the binary was already built.

## Adding a new harness

1. Add `fuzz/fuzz_<target>.cpp` with an `extern "C" int
   LLVMFuzzerTestOneInput(const uint8_t*, size_t)` entry point.
2. Add the matching compile + link block to `.clusterfuzzlite/build.sh`.
3. No workflow changes needed — `cflite_pr.yml`/`cflite_batch.yml`
   build and run every binary `build.sh` produces in `$OUT`.
