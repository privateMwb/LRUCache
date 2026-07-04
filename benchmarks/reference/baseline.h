#pragma once

#include <cstddef>
#include <list>
#include <stdexcept>
#include <unordered_map>
#include <utility>

// Baseline O(1) LRU cache using a doubly-linked list + hash map of
// iterators. List front = most recently used, back = least recently used.
// Serves as a reference/naive implementation to benchmark against.
template<typename K, typename V>
class NaiveLRU {
public:
    // Constructs a cache with the specified capacity.
    explicit NaiveLRU(std::size_t capacity)
        : capacity_(capacity)
    {
        if (capacity == 0)
            throw std::invalid_argument("capacity must be greater than zero");

        // Reserve upfront to avoid measuring hash table growth.
        map_.reserve(capacity_);
    }

    // Inserts or updates an entry.
    void put(const K& key, const V& value) {
        auto it = map_.find(key);

        if (it != map_.end()) {
            it->second->second = value;
            list_.splice(list_.begin(), list_, it->second);
            return;
        }

        if (list_.size() == capacity_) {
            map_.erase(list_.back().first);
            list_.pop_back();
        }

        list_.emplace_front(key, value);
        map_[key] = list_.begin();
    }

    // Retrieves a value and promotes it to MRU.
    V* get(const K& key) {
        auto it = map_.find(key);

        if (it == map_.end())
            return nullptr;

        list_.splice(list_.begin(), list_, it->second);
        return &it->second->second;
    }

    // Removes an entry by key.
    bool erase(const K& key) {
        auto it = map_.find(key);

        if (it == map_.end())
            return false;

        list_.erase(it->second);
        map_.erase(it);

        return true;
    }

    // Returns whether the key exists.
    bool contains(const K& key) const {
        return map_.find(key) != map_.end();
    }

    // Removes all entries.
    void clear() {
        list_.clear();
        map_.clear();
    }

    std::size_t size() const {
        return list_.size();
    }

    std::size_t capacity() const {
        return capacity_;
    }

private:
    std::size_t capacity_;
    std::list<std::pair<K, V>> list_;
    // Maps each key to its node in list_, enabling O(1) lookup, promotion,
    // and eviction without traversing the list.
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map_;
};