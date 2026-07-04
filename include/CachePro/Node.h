#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace CachePro {

// Intrusive doubly-linked list hook shared by all cache nodes.
struct Link {
    Link* prev = nullptr;
    Link* next = nullptr;
};

// Stores a cache entry while embedding the intrusive list links.
// The key/value pair is perfectly forwarded into place.
template<typename K, typename V>
struct Node : Link {
    [[no_unique_address]] K key;
    [[no_unique_address]] V value;

    Node() = default;
    Node(const Node&) = default;
    Node(Node&&) noexcept = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) noexcept = default;
    ~Node() = default;

    template<typename Key, typename Val>
        requires std::constructible_from<K, Key&&> &&
                 std::constructible_from<V, Val&&>
    constexpr Node(Key&& key, Val&& value)
        noexcept(
            std::is_nothrow_constructible_v<K, Key&&> &&
            std::is_nothrow_constructible_v<V, Val&&>
        )
        : key(std::forward<Key>(key))
        , value(std::forward<Val>(value))
    {}
};

} // namespace CachePro