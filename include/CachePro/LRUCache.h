/**
 * @file            LRUCache.h
 *
 * @date            2026-20-7
 *
 * @version         1.0.0
 *
 * @copyright       Copyright (c) 2026 Your Name
 *                  All rights reserved.
 *                  https://github.com/privateMwb/LRUCachePro
 *
 * @attention       This source is released under the MIT license
 *                  SPDX-License-Identifier: MIT
 *                  <http://opensource.org/licenses/MIT>
 */

#pragma once

// clang-format off
#include <cstddef>    // std::size_t, std::byte
#include <functional> // std::hash, std::equal_to
#include <vector>     // std::vector (keys())
// clang-format on

#include "Node.h"

// A fixed-capacity Least Recently Used (LRU) cache. Combines O(1)
// key-to-node lookup with an intrusive doubly linked list to track
// recency and evict in O(1) when full. Nodes live in a single pool
// allocation sized to `capacity`, reused via an embedded free list, so
// steady-state operation performs zero heap allocations.
//
// Key-to-node lookup uses a flat open-addressing table (linear probing,
// backward-shift deletion) rather than a chained hash map: one
// contiguous array, one cache line touched per probe step, and no
// per-entry heap allocation. Because capacity is fixed at construction,
// the table is sized once for a bounded load factor and never grows —
// it only ever gets rebuilt on an explicit resize(). It stores {hash,
// Node*} per slot, not the key itself: the key already lives in the
// pool-allocated Node, so nothing is duplicated.

namespace CachePro {

/**
 * @brief A fixed-capacity LRU cache with O(1) get/put/erase.
 * @tparam K Key type. Must be equality-comparable and hashable by `Hash`.
 * @tparam V Value type.
 * @tparam Hash Hash function object type. Defaults to `std::hash<K>`.
 * @tparam KeyEqual Equality comparator type. Defaults to `std::equal_to<K>`.
 * @details Entries are stored in a pool-allocated array of `Node<K, V>`
 * and linked into an intrusive doubly linked list ordered by recency. A
 * flat open-addressing table maps each key's hash to its node for O(1)
 * lookup, with each node caching its own table slot index so eviction
 * never needs to re-hash or re-probe. Eviction removes the tail of the
 * list (least recently used) and returns its slot to the pool's free list.
 */
template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class LRUCache {
  private:
    /// @brief A pool slot not currently holding a live `Node`, linked into `freeHead_`.
    struct FreeSlot {
        FreeSlot* next;
    };

    /// @brief One slot in the open-addressing table. `node == nullptr`
    /// marks an empty slot; `hash` is cached so probing can reject a
    /// mismatch without touching (and cache-missing on) the node itself.
    struct Slot {
        std::size_t hash = 0;
        Node<K, V>* node = nullptr;
    };

    // Cache state.
    std::size_t capacity_;
    std::size_t size_;
    std::size_t reservedCapacity_;

    // Memory pool.
    std::byte* storage_;
    FreeSlot* freeHead_;

    // Intrusive LRU list.
    Link head_;
    Link tail_;

    // Key-to-node lookup: flat open-addressing table, sized once from
    // `capacity_` at construction (or explicit resize()) — see class docs.
    Slot* table_;
    std::size_t tableCapacity_; ///< Always a power of two.
    std::size_t tableMask_; ///< `tableCapacity_ - 1`; used for `hash & tableMask_` instead of `%`.

    [[no_unique_address]] Hash hasher_;
    [[no_unique_address]] KeyEqual keyEqual_;

    // Cache statistics.
    std::size_t hitCounter_;
    std::size_t missCounter_;

    /**
     * @brief Allocates a raw pool of `count` node-sized slots and threads
     * every slot onto `freeHead_`.
     * @param count Number of `Node<K, V>`-sized slots to allocate.
     */
    void allocatePool(std::size_t count);

    /// @brief Releases the pool allocation. Assumes all nodes were already destroyed.
    void freePool() noexcept;

    /**
     * @brief Allocates a new pool at `newCapacity`, moves every live node
     * into it in list order, and rebuilds the free list from the remainder.
     * @param newCapacity New pool size, in node-sized slots.
     * @details Does not touch the table — nodes keep their old slot
     * indices, now pointing at moved-from addresses, until the caller
     * also calls `rebuildTable()`. `resize()` always does both, in order.
     */
    void rebuildPool(std::size_t newCapacity);

    /**
     * @brief Lowers the logical capacity, evicting down to `newCapacity`.
     * @param newCapacity New logical capacity. Must be greater than 0
     * and no greater than the current capacity.
     * @throws std::invalid_argument if `newCapacity == 0` or
     * `newCapacity > capacity_`.
     * @details Evicts least-recently-used entries until `size_ <=
     * newCapacity`, then sets `capacity_ = newCapacity`. Pool and
     * table are left at their current (larger) physical size — only
     * the logical bound is lowered, so the freed slots are simply
     * never touched again until reused by future `put()` calls. Use
     * `resize()` or `shrink_to_fit()` instead if the memory needs to
     * be physically reclaimed.
     */
    void shrink(std::size_t newCapacity);

    /**
     * @brief Takes a slot off `freeHead_` and constructs a `Node<K, V>` in
     * place from a single key argument and a single value argument.
     * @param key Key to forward into the node.
     * @param value Value to forward into the node.
     * @return Pointer to the newly constructed node. Not yet linked into
     * the list or the table.
     */
    template <typename K2, typename V2> [[nodiscard]] Node<K, V>* acquireNode(K2&& key, V2&& value);

    /**
     * @brief Takes a slot off `freeHead_` and constructs a `Node<K, V>` in
     * place, forwarding `args` directly into the value's constructor.
     * @param key Key to forward into the node.
     * @param args Constructor arguments forwarded directly into the value
     * — no intermediate `V` temporary is built.
     * @return Pointer to the newly constructed node. Not yet linked into
     * the list or the table.
     */
    template <typename K2, typename... Args>
    [[nodiscard]] Node<K, V>* acquireNodeEmplace(K2&& key, Args&&... args);

    /// @brief Destroys `node` and returns its slot to `freeHead_`.
    void releaseNode(Node<K, V>* node) noexcept;

    /// @brief Links `node` in as the most recently used entry.
    void insertFront(Node<K, V>* node) noexcept;

    /// @brief Unlinks `node` from the list without releasing its slot.
    void removeNode(Node<K, V>* node) noexcept;

    /// @brief Moves an already-linked `node` to the front, if not already there.
    void moveToFront(Node<K, V>* node) noexcept;

    /// @brief Unlinks and returns the least recently used node.
    [[nodiscard]] Node<K, V>* popBack() noexcept;

    /// @brief Evicts the least recently used entry from both the list and the table.
    void evictLRU() noexcept;

    /**
     * @brief Rounds up to the table size needed to hold `capacity` entries
     * at a bounded load factor.
     * @param capacity Maximum number of live entries the table must support.
     * @return A power of two, at least 2.
     */
    [[nodiscard]] static std::size_t tableSizeFor(std::size_t capacity) noexcept;

    /// @brief Allocates a zero-initialized (all-empty) table of `count` slots.
    void allocateTable(std::size_t count);

    /// @brief Releases the table allocation.
    void freeTable() noexcept;

    /**
     * @brief Allocates a new table at `newTableCapacity` and reinserts every
     * currently-linked node into it, updating each node's cached slot index.
     * @param newTable Freshly allocated table array of size
     * `newTableCapacity`, ownership transferred in. Allocated by the caller
     * — see @ref resize — so a failed allocation happens before anything is
     * committed, rather than after `rebuildPool()` has already freed the
     * old pool storage and left the old table referencing freed addresses.
     * @param newTableCapacity Size of `newTable`, in slots. Must be a power of two.
     * @details Must run after `rebuildPool()` if both are needed, since it
     * reads node addresses (and re-hashes keys) by walking the current list.
     */
    void rebuildTable(Slot* newTable, std::size_t newTableCapacity);

    /**
     * @brief Applies an avalanche mix to a raw hash before it's used
     * for table placement.
     * @param h Raw hash value, typically straight from `hasher_`.
     * @return A bit-mixed hash with no correlation to `h`'s low bits,
     * so weak or identity-like hashers (e.g. `std::hash<int>` on
     * sequential keys) can't pack the table into one long cluster.
     */
    [[nodiscard]] static std::size_t mixHash(std::size_t h) noexcept;

    /**
     * @brief Computes the mixed hash for `key`.
     * @param key Key to hash.
     * @return `mixHash(hasher_(key))`. All table placement code must
     * go through this rather than calling `hasher_` directly, so every
     * stored `table_[idx].hash` is consistently the mixed value.
     */
    [[nodiscard]] std::size_t computeHash(const K& key) const noexcept(noexcept(hasher_(key)));

    /**
     * @brief Probes the table for `key`.
     * @param key Key to search for.
     * @param hash Precomputed `hasher_(key)`, passed in so callers that
     * already need the hash for another reason don't compute it twice.
     * @return The slot index holding `key`, or `tableCapacity_` (an
     * always-invalid index) if not present.
     */
    [[nodiscard]] std::size_t findSlot(const K& key, std::size_t hash) const noexcept;

    /**
     * @brief Inserts `node` into the table under `hash`, linear-probing
     * from its home slot to the first empty slot.
     * @param hash Precomputed `hasher_(node->key)`.
     * @param node Node to insert. Its `slot` member is updated in place.
     * @details Caller must have already confirmed `node`'s key isn't
     * already present — this never checks, only finds an empty slot.
     */
    void insertSlot(std::size_t hash, Node<K, V>* node) noexcept;

    /**
     * @brief Removes the entry at table slot `index` via backward-shift
     * deletion, without leaving a tombstone.
     * @param index Slot to vacate. Must currently hold a live entry.
     * @details Walks forward from `index`, pulling back any subsequent
     * entry whose probe sequence still reaches the new hole, until it
     * finds one that can't move (or an empty slot). Keeps future probes
     * short forever — no tombstone accumulation, unlike lazy-deletion
     * open addressing.
     */
    void eraseSlot(std::size_t index) noexcept;

    /**
     * @brief Shared implementation for all four `put()` overloads.
     * @param key Key to insert or update.
     * @param value Value to insert or update.
     * @details On update, only the value is replaced. On insert, evicts the
     * least recently used entry first if the cache is at capacity. The new
     * node is fully constructed before it's linked into the table or list,
     * so a throwing `K`/`V` constructor leaves the cache unchanged.
     */
    template <typename K2, typename V2> void putImpl(K2&& key, V2&& value);

  public:
    /**
     * @brief Constructs an empty cache with room for `capacity` entries.
     * @param capacity Maximum number of entries. Must be greater than 0.
     * @throws std::invalid_argument if `capacity` is 0.
     */
    explicit LRUCache(std::size_t capacity);

    /// @brief Destroys all entries and releases the pool and table.
    ~LRUCache();

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    /**
     * @brief Move-constructs a cache, taking ownership of `other`'s pool and table.
     * @param other Cache to move from. Left empty and reusable.
     */
    LRUCache(LRUCache&& other) noexcept;

    /**
     * @brief Move-assigns from `other`, replacing this cache's contents.
     * @param other Cache to move from. Left empty and reusable.
     * @return Reference to `*this`.
     */
    LRUCache& operator=(LRUCache&& other) noexcept;

    /// @brief Inserts or updates `key` with `value`, copying both.
    void put(const K& key, const V& value);
    /// @brief Inserts or updates `key` with `value`, copying the key and moving the value.
    void put(const K& key, V&& value);
    /// @brief Inserts or updates `key` with `value`, moving the key and copying the value.
    void put(K&& key, const V& value);
    /// @brief Inserts or updates `key` with `value`, moving both.
    void put(K&& key, V&& value);

    /**
     * @brief Inserts `key` with a value constructed in place from `args` if
     * not already present, or replaces the existing value with a new
     * `V(args...)` otherwise.
     * @param key Key to look up or insert.
     * @param args Constructor arguments for the value. On the insert path
     * these are forwarded straight into the value's constructor with no
     * intermediate `V` temporary.
     * @return Reference to the (possibly newly inserted) value.
     */
    template <typename... Args> V& emplace(const K& key, Args&&... args);

    /**
     * @brief Removes the entry mapped to `key`, if present.
     * @param key Key to remove.
     * @return `true` if `key` was found and removed, `false` otherwise.
     */
    [[nodiscard]] bool erase(const K& key);

    /// @brief Removes all entries. Capacity is unchanged.
    void clear() noexcept;

    /**
     * @brief Looks up `key`, marking it most recently used on a hit.
     * @param key Key to search for.
     * @return Pointer to the mapped value, or `nullptr` if not present.
     */
    [[nodiscard]] V* get(const K& key);

    /**
     * @brief Looks up `key` without affecting recency order.
     * @param key Key to search for.
     * @return Pointer to the mapped value, or `nullptr` if not present.
     */
    [[nodiscard]] const V* peek(const K& key) const;

    /**
     * @brief Checks whether `key` is present, without affecting recency order.
     * @param key Key to search for.
     * @return `true` if `key` is present.
     */
    [[nodiscard]] bool contains(const K& key) const;

    /**
     * @brief Changes capacity, evicting least-recently-used entries first if
     * shrinking, and rebuilds the pool and table at the new size.
     * @param newCapacity New maximum number of entries. Must be greater than 0.
     * @throws std::invalid_argument if `newCapacity` is 0.
     */
    void resize(std::size_t newCapacity);

    /**
     * @brief No-op: the table is sized for `capacity_` once at construction
     * and never needs preallocation ahead of inserts. Kept for API
     * compatibility with map-backed cache implementations.
     * @param count Ignored.
     */
    void reserve(std::size_t count);

    /**
     * @brief Reclaims unused capacity, shrinking storage down to the
     * current size.
     *
     * Reduces `capacity()` to `size()` and physically rebuilds the
     * pool and table to match. This is the inverse of `shrink()`,
     * which lowers the capacity bound but leaves storage oversized —
     * `shrink_to_fit()` actually reclaims that memory. No-op if the
     * cache is empty or already at capacity. Never evicts, since
     * there's nothing beyond the current size to shrink below.
     */
    void shrink_to_fit();

    /// @brief Returns all keys, ordered most recently used first.
    [[nodiscard]] std::vector<K> keys() const;
    /// @brief Returns a pointer to the most recently used key, or `nullptr` if empty.
    [[nodiscard]] const K* mostRecentKey() const noexcept;
    /// @brief Returns a pointer to the least recently used key, or `nullptr` if empty.
    [[nodiscard]] const K* leastRecentKey() const noexcept;

    /// @brief Returns the number of `get()` calls that found their key.
    [[nodiscard]] std::size_t hitCount() const noexcept;
    /// @brief Returns the number of `get()` calls that did not find their key.
    [[nodiscard]] std::size_t missCount() const noexcept;
    /// @brief Returns `hitCount() / (hitCount() + missCount())` as a percentage, or `0.0` if no
    /// lookups yet.
    [[nodiscard]] double hitRate() const noexcept;
    /// @brief Resets `hitCount()` and `missCount()` to 0.
    void resetStats() noexcept;

    /// @brief Returns the maximum number of entries this cache can hold.
    [[nodiscard]] std::size_t capacity() const noexcept;
    /// @brief Returns the number of entries currently stored.
    [[nodiscard]] std::size_t size() const noexcept;
    /// @brief Returns whether the cache has no entries.
    [[nodiscard]] bool empty() const noexcept;
};

} // namespace CachePro

/// @brief Short alias so this library can be used as `rain::LRUCache`, while
/// its true namespace (and all internal diagnostics) remains `CachePro`.
/// See Node.h for the same alias applied to `rain::Node`.
namespace rain = CachePro;

#include "LRUCache.tpp"
