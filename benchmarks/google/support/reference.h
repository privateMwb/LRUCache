/**
 * @file StdLRU.h
 * @brief Baseline LRU cache used as the comparison target across the
 * LRUCache benchmark suite.
 *
 * Built from std::list<std::pair<K, V>> (recency order) plus
 * std::unordered_map<K, list::iterator> (key lookup), the conventional
 * way an LRU cache is built in C++. Exposes the same operations as
 * CachePro::LRUCache so both sides can be driven from identical lambdas.
 */

#pragma once

#include <cstddef>
#include <list>
#include <unordered_map>
#include <utility>
#include <vector>

namespace bench {

template <typename K, typename V> class stdLRU {
  public:
    explicit stdLRU(std::size_t capacity) : capacity_(capacity) {}

    void put(const K& key, const V& value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;
            list_.splice(list_.begin(), list_, it->second);
            return;
        }
        if (list_.size() >= capacity_) {
            map_.erase(list_.back().first);
            list_.pop_back();
        }
        list_.emplace_front(key, value);
        map_[key] = list_.begin();
    }

    V* get(const K& key) {
        auto it = map_.find(key);
        if (it == map_.end())
            return nullptr;
        list_.splice(list_.begin(), list_, it->second);
        return &it->second->second;
    }

    // Rough equivalent of LRUCache::emplace() — std::list/std::unordered_map
    // have no native in-place-keyed-insert primitive, so this constructs V
    // directly from args and reuses the put() insert/update path.
    template <typename... Args> V& emplace(const K& key, Args&&... args) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = V(std::forward<Args>(args)...);
            list_.splice(list_.begin(), list_, it->second);
            return it->second->second;
        }
        if (list_.size() >= capacity_) {
            map_.erase(list_.back().first);
            list_.pop_back();
        }
        list_.emplace_front(key, V(std::forward<Args>(args)...));
        map_[key] = list_.begin();
        return list_.front().second;
    }

    const V* peek(const K& key) const {
        auto it = map_.find(key);
        if (it == map_.end())
            return nullptr;
        return &it->second->second;
    }

    bool contains(const K& key) const {
        return map_.find(key) != map_.end();
    }

    bool erase(const K& key) {
        auto it = map_.find(key);
        if (it == map_.end())
            return false;
        list_.erase(it->second);
        map_.erase(it);
        return true;
    }

    void clear() noexcept {
        list_.clear();
        map_.clear();
    }

    // Evicts down to newCapacity if shrinking; just raises the ceiling if growing.
    void resize(std::size_t newCapacity) {
        while (list_.size() > newCapacity) {
            map_.erase(list_.back().first);
            list_.pop_back();
        }
        capacity_ = newCapacity;
        map_.reserve(newCapacity);
    }

    void reserve(std::size_t count) {
        map_.reserve(count);
    }

    std::vector<K> keys() const {
        std::vector<K> out;
        out.reserve(list_.size());
        for (const auto& kv : list_)
            out.push_back(kv.first);
        return out;
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return list_.size();
    }
    [[nodiscard]] bool empty() const noexcept {
        return list_.empty();
    }
    [[nodiscard]] std::size_t capacity() const noexcept {
        return capacity_;
    }

  private:
    std::size_t capacity_;
    std::list<std::pair<K, V>> list_;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map_;
};

} // namespace bench
