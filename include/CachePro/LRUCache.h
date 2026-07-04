#pragma once

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

#include "Node.h"

namespace CachePro {

// A fixed-capacity Least Recently Used (LRU) cache.
// Combines O(1) hash-table lookup with an intrusive doubly linked list
// to evict the least recently used entry when the cache is full.
template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
class LRUCache {
private:
    struct FreeSlot { FreeSlot* next; };

    // Cache state.
    std::size_t capacity_;
    std::size_t size_;

    // Memory pool.
    std::byte* storage_;
    FreeSlot*  freeHead_;

    // Intrusive LRU list.
    Link head_;
    Link tail_;

    // Key-to-node lookup.
    std::unordered_map<K, Node<K, V>*, Hash, KeyEqual> cache_;

    // Cache statistics.
    std::size_t hitCounter_;
    std::size_t missCounter_;

    // Memory pool management.
    void allocatePool(std::size_t count);
    void freePool() noexcept;
    void rebuildPool(std::size_t newCapacity);

    template<typename K2, typename V2>
    [[nodiscard]] Node<K, V>* acquireNode(K2&& key, V2&& value);
    void releaseNode(Node<K, V>* node) noexcept;

    // LRU list management.
    void insertFront(Node<K, V>* node) noexcept;
    void removeNode(Node<K, V>* node) noexcept;
    void moveToFront(Node<K, V>* node) noexcept;
    [[nodiscard]] Node<K, V>* popBack() noexcept;
    void evictLRU() noexcept;

    // Internal helpers.
    template<typename K2, typename V2>
    void putImpl(K2&& key, V2&& value);

public:
    // Constructors & Destructor.
    explicit LRUCache(std::size_t capacity);
    ~LRUCache();

    LRUCache(const LRUCache&)            = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    LRUCache(LRUCache&& other) noexcept;
    LRUCache& operator=(LRUCache&& other) noexcept;

    // Modifiers.
    void put(const K& key, const V& value);
    void put(const K& key, V&& value);
    void put(K&& key, const V& value);
    void put(K&& key, V&& value);

    template<typename... Args>
    V& emplace(const K& key, Args&&... args);

    [[nodiscard]] bool erase(const K& key);
    void clear() noexcept;

    // Lookup.
    [[nodiscard]] V* get(const K& key);
    [[nodiscard]] const V* peek(const K& key) const;
    [[nodiscard]] bool contains(const K& key) const;

    // Capacity Management.
    void resize(std::size_t newCapacity);
    void reserve(std::size_t count);

    // Observers.
    [[nodiscard]] std::vector<K> keys() const;
    [[nodiscard]] const K* mostRecentKey() const noexcept;
    [[nodiscard]] const K* leastRecentKey() const noexcept;

    // Statistics.
    [[nodiscard]] std::size_t hitCount() const noexcept;
    [[nodiscard]] std::size_t missCount() const noexcept;
    [[nodiscard]] double hitRate() const noexcept;
    void resetStats() noexcept;

    // Capacity.
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
};

} // namespace CachePro

#include "LRUCache.tpp"