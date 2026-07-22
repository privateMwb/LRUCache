/**
 * @file Node.h
 * @brief Internal node structure used by LRUCache.
 *
 * Contains the node implementation used internally by the LRUCache
 * container for key-value storage and intrusive recency-list linkage.
 */

#pragma once

// clang-format off
#include <concepts>    // std::constructible_from
#include <cstddef>     // std::size_t
#include <type_traits> // std::is_nothrow_constructible_v
#include <utility>     // std::forward, std::in_place_t
// clang-format on

namespace CachePro {

/**
 * @brief Intrusive doubly-linked list hook shared by all cache nodes.
 * @details Embedded as the base of every `Node`, so a `Node*` can be used
 * directly as a `Link*` in list operations without an extra indirection or
 * a separate list-node allocation.
 */
struct Link {
    Link* prev = nullptr; ///< Previous entry in the list, or the sentinel if this is the front.
    Link* next = nullptr; ///< Next entry in the list, or the sentinel if this is the back.
};

/**
 * @brief A single cache entry: a key/value pair, the intrusive list links
 * needed to track its position in recency order, and its current slot
 * index in `LRUCache`'s open-addressing table.
 * @tparam K Key type.
 * @tparam V Value type.
 * @details Inherits `Link` rather than storing one, so `LRUCache` can
 * `static_cast` between `Node*` and `Link*` freely and walk the recency
 * list without touching `key` or `value` at all. Every `Node` lives in a
 * pool slot owned by `LRUCache`; construction and destruction happen via
 * placement new / explicit destructor calls at that slot, never through
 * ordinary `new`/`delete`.
 */
template <typename K, typename V> struct Node : Link {
    [[no_unique_address]] K key;   ///< The stored key.
    [[no_unique_address]] V value; ///< The stored value associated with `key`.

    /// @brief This node's current index in `LRUCache`'s table, kept in sync
    /// by every table operation (insert, erase, backward-shift) so eviction
    /// and explicit `erase()` never need to re-hash or re-probe by key.
    std::size_t slot = 0;

    /// @brief Default-constructs `key` and `value`. Used only to placement-construct into a raw
    /// pool slot.
    Node() = default;

    Node(const Node&) = default;
    Node(Node&&) noexcept = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) noexcept = default;
    ~Node() = default;

    /**
     * @brief Constructs `key` and `value` in place from a single key
     * argument and a single value argument.
     * @tparam Key Deduced type of the key argument.
     * @tparam Val Deduced type of the value argument.
     * @param key Forwarded into `K`'s constructor.
     * @param value Forwarded into `V`'s constructor.
     * @details Constrained via `std::constructible_from` so overload
     * resolution fails cleanly (rather than deep in the member-initializer
     * list) when `Key`/`Val` can't construct `K`/`V`. `noexcept` is computed
     * from the actual constructors selected, so `LRUCache` can tell at
     * compile time whether acquiring a node can throw.
     */
    template <typename Key, typename Val>
        requires std::constructible_from<K, Key&&> && std::constructible_from<V, Val&&>
    constexpr Node(Key&& key, Val&& value) noexcept(std::is_nothrow_constructible_v<K, Key&&> &&
                                                    std::is_nothrow_constructible_v<V, Val&&>)
        : key(std::forward<Key>(key)), value(std::forward<Val>(value)) {}

    /**
     * @brief Constructs `key` in place and `value` in place from an
     * arbitrary argument pack, with no intermediate `V` temporary.
     * @tparam Key Deduced type of the key argument.
     * @tparam Args Deduced types of `value`'s constructor arguments.
     * @param key Forwarded into `K`'s constructor.
     * @param args Forwarded directly into `V`'s constructor.
     * @details Takes a leading `std::in_place_t` tag purely to disambiguate
     * from the two-argument (`Key`, `Val`) constructor above when `Args`
     * happens to be a single argument — without the tag, `Node(k, v)` would
     * be ambiguous between "construct value from `v`" and "construct value
     * in place from the one-element pack `{v}}`. Lets `LRUCache::emplace()`
     * forward its variadic arguments straight into `value` instead of
     * building a `V` temporary and moving it in.
     */
    template <typename Key, typename... Args>
        requires std::constructible_from<K, Key&&> && std::constructible_from<V, Args&&...>
    constexpr Node(Key&& key, std::in_place_t,
                   Args&&... args) noexcept(std::is_nothrow_constructible_v<K, Key&&> &&
                                            std::is_nothrow_constructible_v<V, Args&&...>)
        : key(std::forward<Key>(key)), value(std::forward<Args>(args)...) {}
};

} // namespace CachePro

/// @brief Short alias so this library can be used as `rain::Node`, while its
/// true namespace (and all internal diagnostics) remains `CachePro`. See
/// LRUCache.h for the same alias applied to `rain::LRUCache`.
namespace rain = CachePro;
