#pragma once 

#include <cstddef>
#include <unordered_map>

#include "Node.h"

template<typename K, typename V, typename Hash = std::hash<K>> // K = Key, V = Value
class LRUCache {
    private:
    // Capacity 
    std::size_t cap;
    
    // Sentinel Nodes
    Node<K, V>* head = nullptr;
    Node<K, V>* tail = nullptr;
    
    // Lookup Table
    std::unordered_map<K, Node<K, V>*, Hash> cache;
    
    // Cache Statistics
    std::size_t hitCounter = 0;
    std::size_t missCounter = 0;
    
    // List Operations
    void insertFront(Node<K, V>* node);
    void removeNode(Node<K, V>* node);
    void moveToFront(Node<K, V>* node);
    Node<K, V>* popBack();
    
    // Lookup Helpers
    Node<K, V>* findNode(const K& key) const;
    Node<K, V>* findKey(const K& key);
    
    // Resource Management 
    void release() noexcept;
    
    public:
    // Lifecycle
    explicit LRUCache(std::size_t cap);
    ~LRUCache();
    
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
    
    LRUCache(LRUCache&& other) noexcept;
    LRUCache& operator=(LRUCache&& other) noexcept;
    
    // Cache Operations
    void put(const K& key, const V& value);
    V* get(const K& key);
    
    bool erase(const K& key);
    void clear();
    
    bool contain(const K& key) const;
    
    // Cache Access
    const V* peek(const K& key) const;
    
    // Capacity Management
    void reserve(std::size_t count);
    
    // Cache Statistics
    std::size_t hitCount() const noexcept;
    std::size_t missCount() const noexcept;
    double hitRate() const noexcept;
    
    // LRU Order Inspection
    const K* mostRecentKey() const;
    const K* leastRecentKey() const;
    
    // Capacity
    std::size_t capacity() const noexcept;
    std::size_t size() const noexcept;
    
    // State
    bool empty() const noexcept;
};

#include "LRUCache.tpp"