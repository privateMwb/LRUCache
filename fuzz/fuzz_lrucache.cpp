// ============================================================
// fuzz/fuzz_lrucache.cpp
//
// Differential fuzzer for CachePro::LRUCache<int, int>, checked after
// every single operation (not just at the end) against a hand-rolled
// shadow model — a std::list<std::pair<int,int>> kept in the same
// most-recently-used-first order the real cache is documented to
// maintain. A failing input therefore localizes to the exact
// operation that broke an invariant, not just "somewhere in this run".
//
// int is used for both K and V deliberately, not for simplicity: it
// makes every put()/emplace()/erase() cheap enough that a single fuzz
// run can drive thousands of insert/evict/erase cycles, which is what
// actually stresses the two hand-written subsystems LRUCache leans on
// instead of standard containers:
//
//   - The pool's free list (acquireNode()/acquireNodeEmplace()/
//     releaseNode()) under repeated evict-then-insert and
//     erase-then-insert cycles — this is what pool_reuse.cpp and
//     full_load_guard.cpp probe by hand, but only for a few
//     hand-picked sequences. The fuzzer explores orderings a person
//     wouldn't think to write.
//   - The open-addressing table's linear probing and backward-shift
//     deletion (findSlot()/insertSlot()/eraseSlot()) — erase_slot_
//     wraparound.cpp hand-constructs one wraparound case with a
//     colliding hash; here, small capacities (biased low, same as
//     VectorPro's fuzzer biases toward forcing growth) combined with
//     std::hash<int>'s real distribution produce wraparound clusters
//     "for free" across many different key sequences.
//
// Specifically exercised:
//   - Insert/update correctness and MRU-front placement for both
//     put() and emplace()
//   - Eviction order under put()/emplace() at capacity, and under
//     resize() shrinking below the live entry count
//   - get() vs peek() vs contains(): exactly one of these three should
//     ever move an entry to MRU or touch hitCount()/missCount()
//   - resize()'s lazy-path/real-growth split (growing, shrinking above
//     the live count, shrinking below it) and resize(0)'s contract:
//     throws std::invalid_argument and leaves the cache byte-for-byte
//     unchanged
//   - reserve() and shrink_to_fit()'s no-op / reclaim contracts
//   - Move-construct and move-assign, including self-move-assignment,
//     always performed on a scratch cache and moved back — see "What's
//     deliberately NOT covered yet" in FUZZING.md for why the
//     persistent cache under test is never left moved-from
//
// Deliberately NOT covered here: custom Hash/KeyEqual template
// parameters (only the defaults are exercised), exception injection
// via a throwing K or V (int can't throw, so acquireNode()'s catch
// path is never hit), and put()/get() on an already-moved-from cache
// — see FUZZING.md.
// ============================================================

#include <CachePro/LRUCache.h>

#include <cstdint>
#include <cstdlib>
#include <list>
#include <utility>
#include <vector>

using CachePro::LRUCache;

namespace {

using Shadow = std::list<std::pair<int, int>>; // front = MRU, back = LRU

// Aborts (rather than throwing/returning) on mismatch so libFuzzer
// captures a minimal, precise reproducer for exactly the operation
// that broke an invariant.
void verify(const LRUCache<int, int>& cache, const Shadow& shadow, std::size_t shadowCapacity) {
    if (cache.size() != shadow.size())
        std::abort();
    if (cache.capacity() != shadowCapacity)
        std::abort();
    if (cache.empty() != shadow.empty())
        std::abort();

    if (shadow.empty()) {
        if (cache.mostRecentKey() != nullptr || cache.leastRecentKey() != nullptr)
            std::abort();
    } else {
        const int* mru = cache.mostRecentKey();
        const int* lru = cache.leastRecentKey();
        if (mru == nullptr || lru == nullptr)
            std::abort();
        if (*mru != shadow.front().first || *lru != shadow.back().first)
            std::abort();
    }

    std::vector<int> keys = cache.keys();
    if (keys.size() != shadow.size())
        std::abort();
    auto shadowIt = shadow.begin();
    for (std::size_t i = 0; i < keys.size(); ++i, ++shadowIt) {
        if (keys[i] != shadowIt->first)
            std::abort();
    }

    // peek() every live entry too — catches a table/list desync that
    // keys() alone (list-only) wouldn't.
    for (const auto& [key, value] : shadow) {
        const int* v = cache.peek(key);
        if (v == nullptr || *v != value)
            std::abort();
    }
}

// Moves `key` to the front of `shadow` if present, without changing its value.
void shadowTouch(Shadow& shadow, int key) {
    for (auto it = shadow.begin(); it != shadow.end(); ++it) {
        if (it->first == key) {
            shadow.splice(shadow.begin(), shadow, it);
            return;
        }
    }
}

// Mirrors putImpl(): update-in-place + move-to-front, or insert-at-front
// with LRU eviction if already at capacity.
void shadowPut(Shadow& shadow, std::size_t capacity, int key, int value) {
    for (auto it = shadow.begin(); it != shadow.end(); ++it) {
        if (it->first == key) {
            it->second = value;
            shadow.splice(shadow.begin(), shadow, it);
            return;
        }
    }
    if (shadow.size() >= capacity) {
        shadow.pop_back();
    }
    shadow.push_front({key, value});
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    if (size == 0)
        return 0;

    // First byte selects a starting capacity, biased toward small
    // values so most runs force eviction and table-wraparound activity
    // almost immediately rather than only exercising a roomy cache.
    static constexpr std::size_t kCapacities[] = {1, 2, 3, 4, 8, 32};
    std::size_t capacity = kCapacities[data[0] % 6];
    ++data;
    --size;

    LRUCache<int, int> cache(capacity);
    Shadow shadow;
    std::size_t shadowHits = 0;
    std::size_t shadowMisses = 0;

    for (std::size_t i = 0; i < size; ++i) {
        const std::uint8_t op = data[i] % 15;
        const int key = static_cast<int>(data[i]);
        const int value = static_cast<int>(i);

        switch (op) {
        case 0: { // put() — insert or update
            cache.put(key, value);
            shadowPut(shadow, capacity, key, value);
            break;
        }
        case 1: { // emplace() — same insert-or-update contract as put()
            cache.emplace(key, value);
            shadowPut(shadow, capacity, key, value);
            break;
        }
        case 2: { // get() — hit updates MRU and hitCount(); miss updates missCount()
            int* got = cache.get(key);
            bool foundInShadow = false;
            for (const auto& [k, v] : shadow) {
                if (k == key) {
                    foundInShadow = true;
                    if (got == nullptr || *got != v)
                        std::abort();
                    break;
                }
            }
            if (got == nullptr && foundInShadow)
                std::abort();
            if (foundInShadow) {
                shadowTouch(shadow, key);
                ++shadowHits;
            } else {
                if (got != nullptr)
                    std::abort();
                ++shadowMisses;
            }
            if (cache.hitCount() != shadowHits || cache.missCount() != shadowMisses)
                std::abort();
            break;
        }
        case 3: { // peek() — must never move MRU or touch hit/miss counters
            const int* peeked = cache.peek(key);
            bool foundInShadow = false;
            for (const auto& [k, v] : shadow) {
                if (k == key) {
                    foundInShadow = true;
                    if (peeked == nullptr || *peeked != v)
                        std::abort();
                    break;
                }
            }
            if ((peeked != nullptr) != foundInShadow)
                std::abort();
            if (cache.hitCount() != shadowHits || cache.missCount() != shadowMisses)
                std::abort();
            break;
        }
        case 4: { // contains() — same no-side-effect contract as peek()
            bool foundInCache = cache.contains(key);
            bool foundInShadow = false;
            for (const auto& [k, v] : shadow) {
                if (k == key) {
                    foundInShadow = true;
                    break;
                }
            }
            if (foundInCache != foundInShadow)
                std::abort();
            if (cache.hitCount() != shadowHits || cache.missCount() != shadowMisses)
                std::abort();
            break;
        }
        case 5: { // erase()
            bool erasedInCache = cache.erase(key);
            bool erasedInShadow = false;
            for (auto it = shadow.begin(); it != shadow.end(); ++it) {
                if (it->first == key) {
                    shadow.erase(it);
                    erasedInShadow = true;
                    break;
                }
            }
            if (erasedInCache != erasedInShadow)
                std::abort();
            break;
        }
        case 6: { // clear() — capacity unchanged, contents gone
            cache.clear();
            shadow.clear();
            break;
        }
        case 7: { // resize() — grow, shrink above live count, or shrink below it
            std::size_t newCapacity = (static_cast<std::size_t>(data[i]) % 16) + 1;
            cache.resize(newCapacity);
            while (shadow.size() > newCapacity) {
                shadow.pop_back();
            }
            capacity = newCapacity;
            break;
        }
        case 8: { // resize(0) — must throw and leave the cache byte-for-byte unchanged
            std::size_t sizeBefore = cache.size();
            std::size_t capacityBefore = cache.capacity();
            bool threw = false;
            try {
                cache.resize(0);
            } catch (const std::invalid_argument&) {
                threw = true;
            }
            if (!threw)
                std::abort();
            if (cache.size() != sizeBefore || cache.capacity() != capacityBefore)
                std::abort();
            break;
        }
        case 9: { // reserve() — documented no-op regardless of argument
            cache.reserve(static_cast<std::size_t>(data[i]));
            // capacity/size/contents must be exactly as before — verify() below confirms it.
            break;
        }
        case 10: { // shrink_to_fit() — capacity becomes size(); no-op if empty or already full
            cache.shrink_to_fit();
            if (!shadow.empty() && shadow.size() != capacity) {
                capacity = shadow.size();
            }
            break;
        }
        case 11: { // resetStats() — zeroes both counters
            cache.resetStats();
            shadowHits = 0;
            shadowMisses = 0;
            break;
        }
        case 12: { // self-move-assignment — must be a safe no-op
            LRUCache<int, int>& selfRef = cache;
            cache = std::move(selfRef);
            break;
        }
        case 13: { // move-assign from a freshly built scratch cache (state transfer)
            LRUCache<int, int> scratch(capacity);
            // shadow is MRU-first; scratch.put() makes the just-inserted
            // key MRU, so iterate LRU-to-MRU (reverse) to land on the
            // same recency order as shadow, not the reverse of it.
            for (auto it = shadow.rbegin(); it != shadow.rend(); ++it) {
                scratch.put(it->first, it->second);
            }
            cache = std::move(scratch);
            // scratch was fresh, so its (and now cache's) hit/miss
            // counters are zero -- the shadow's tracking must follow.
            shadowHits = 0;
            shadowMisses = 0;
            // scratch is now moved-from and immediately discarded — never touched again.
            break;
        }
        case 14: { // move-construct round-trip — exercises the ctor without losing `cache`
            LRUCache<int, int> moved(std::move(cache));
            cache = std::move(moved);
            // `moved` is now moved-from and immediately discarded — never touched again.
            break;
        }
        default:
            break;
        }

        verify(cache, shadow, capacity);
    }

    return 0;
}
