// ============================================================
// LRUCache.tpp
// Template implementation for CachePro::LRUCache.
// ============================================================
//
// Sections:
//   1. Pool Management
//   2. Node Management
//   3. LRU List Management
//   4. Constructors & Destructor
//   5. Modifiers
//   6. Lookup
//   7. Capacity Management
//   8. Observers
//   9. Statistics
//  10. Capacity Queries
//
// ============================================================

#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

namespace CachePro {


// ============================================================
// Section 1 — Pool Management
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::allocatePool(std::size_t count) {
    storage_ = static_cast<std::byte*>(
        ::operator new(count * sizeof(Node<K, V>), std::align_val_t(alignof(Node<K, V>))));

    freeHead_ = nullptr;
    for (std::size_t i = count; i-- > 0;) {
        FreeSlot* slot = std::construct_at(
            reinterpret_cast<FreeSlot*>(storage_ + i * sizeof(Node<K, V>)), FreeSlot{freeHead_});
        freeHead_ = slot;
    }
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::freePool() noexcept {
    ::operator delete(storage_, std::align_val_t(alignof(Node<K, V>)));
    storage_  = nullptr;
    freeHead_ = nullptr;
}


// ============================================================
// Section 2 — Node Management
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
template<typename K2, typename V2>
Node<K, V>* LRUCache<K, V, Hash, KeyEqual>::acquireNode(K2&& key, V2&& value) {
    FreeSlot* slot = freeHead_;
    freeHead_ = slot->next;
    std::destroy_at(slot);

    Node<K, V>* node = std::construct_at(
        reinterpret_cast<Node<K, V>*>(slot), std::forward<K2>(key), std::forward<V2>(value));

    ++size_;
    return node;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::releaseNode(Node<K, V>* node) noexcept {
    std::destroy_at(node);
    FreeSlot* slot = std::construct_at(reinterpret_cast<FreeSlot*>(node), FreeSlot{freeHead_});
    freeHead_ = slot;
    --size_;
}


// ============================================================
// Section 3 — LRU List Management
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::insertFront(Node<K, V>* node) noexcept {
    Link* n = node;
    n->next = head_.next;
    n->prev = &head_;
    head_.next->prev = n;
    head_.next       = n;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::removeNode(Node<K, V>* node) noexcept {
    Link* n = node;
    n->prev->next = n->next;
    n->next->prev = n->prev;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::moveToFront(Node<K, V>* node) noexcept {
    if (head_.next == node) [[unlikely]] {
        return;
    }
    removeNode(node);
    insertFront(node);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
Node<K, V>* LRUCache<K, V, Hash, KeyEqual>::popBack() noexcept {
    Node<K, V>* node = static_cast<Node<K, V>*>(tail_.prev);
    removeNode(node);
    return node;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::evictLRU() noexcept {
    Node<K, V>* lru = popBack();
    cache_.erase(lru->key);
    releaseNode(lru);
}


// ============================================================
// Section 4 — Constructors & Destructor
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>::LRUCache(std::size_t capacity) :
    capacity_(capacity),
    size_(0),
    storage_(nullptr),
    freeHead_(nullptr),
    head_{nullptr, nullptr},
    tail_{nullptr, nullptr},
    hitCounter_(0),
    missCounter_(0)
{
    if (capacity == 0) {
        throw std::invalid_argument("LRUCache capacity must be greater than 0");
    }

    head_.next = &tail_;
    tail_.prev = &head_;

    allocatePool(capacity_);
    cache_.reserve(capacity_);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>::~LRUCache() {
    clear();
    freePool();
}

template<typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>::LRUCache(LRUCache&& other) noexcept :
    capacity_(other.capacity_),
    size_(other.size_),
    storage_(other.storage_),
    freeHead_(other.freeHead_),
    head_{nullptr, nullptr},
    tail_{nullptr, nullptr},
    cache_(std::move(other.cache_)),
    hitCounter_(other.hitCounter_),
    missCounter_(other.missCounter_)
{
    if (other.head_.next == &other.tail_) {
        head_.next = &tail_;
        tail_.prev = &head_;
    } else {
        head_.next = other.head_.next;
        tail_.prev = other.tail_.prev;
        head_.next->prev = &head_;
        tail_.prev->next = &tail_;
    }

    other.capacity_    = 0;
    other.size_        = 0;
    other.storage_     = nullptr;
    other.freeHead_    = nullptr;
    other.head_.next   = &other.tail_;
    other.tail_.prev   = &other.head_;
    other.hitCounter_  = 0;
    other.missCounter_ = 0;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>& LRUCache<K, V, Hash, KeyEqual>::operator=(LRUCache&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    clear();
    freePool();

    capacity_    = other.capacity_;
    size_        = other.size_;
    storage_     = other.storage_;
    freeHead_    = other.freeHead_;
    cache_       = std::move(other.cache_);
    hitCounter_  = other.hitCounter_;
    missCounter_ = other.missCounter_;

    if (other.head_.next == &other.tail_) {
        head_.next = &tail_;
        tail_.prev = &head_;
    } else {
        head_.next = other.head_.next;
        tail_.prev = other.tail_.prev;
        head_.next->prev = &head_;
        tail_.prev->next = &tail_;
    }

    other.capacity_    = 0;
    other.size_        = 0;
    other.storage_     = nullptr;
    other.freeHead_    = nullptr;
    other.head_.next   = &other.tail_;
    other.tail_.prev   = &other.head_;
    other.hitCounter_  = 0;
    other.missCounter_ = 0;

    return *this;
}


// ============================================================
// Section 5 — Modifiers
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
template<typename K2, typename V2>
void LRUCache<K, V, Hash, KeyEqual>::putImpl(K2&& key, V2&& value) {
    auto [it, inserted] = cache_.try_emplace(std::forward<K2>(key), nullptr);

    if (!inserted) {
        it->second->value = std::forward<V2>(value);
        moveToFront(it->second);
        return;
    }

    if (size_ == capacity_) [[unlikely]] {
        evictLRU();
    }

    Node<K, V>* node = acquireNode(it->first, std::forward<V2>(value));
    it->second = node;
    insertFront(node);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(const K& key, const V& value) { putImpl(key, value); }

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(const K& key, V&& value) { putImpl(key, std::move(value)); }

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(K&& key, const V& value) { putImpl(std::move(key), value); }

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(K&& key, V&& value) { putImpl(std::move(key), std::move(value)); }

template<typename K, typename V, typename Hash, typename KeyEqual>
template<typename... Args>
V& LRUCache<K, V, Hash, KeyEqual>::emplace(const K& key, Args&&... args) {
    auto it = cache_.find(key);

    if (it != cache_.end()) {
        it->second->value = V(std::forward<Args>(args)...);
        moveToFront(it->second);
        return it->second->value;
    }

    if (size_ == capacity_) [[unlikely]] {
        evictLRU();
    }

    Node<K, V>* node = acquireNode(key, V(std::forward<Args>(args)...));
    cache_.emplace(node->key, node);
    insertFront(node);
    return node->value;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool LRUCache<K, V, Hash, KeyEqual>::erase(const K& key) {
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return false;
    }

    Node<K, V>* node = it->second;
    removeNode(node);
    cache_.erase(it);
    releaseNode(node);
    return true;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::clear() noexcept {
    Link* current = head_.next;

    while (current != &tail_) {
        Node<K, V>* node = static_cast<Node<K, V>*>(current);
        current = current->next;
        releaseNode(node);
    }

    head_.next = &tail_;
    tail_.prev = &head_;
    cache_.clear();
}


// ============================================================
// Section 6 — Lookup
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
V* LRUCache<K, V, Hash, KeyEqual>::get(const K& key) {
    auto it = cache_.find(key);

    if (it == cache_.end()) [[unlikely]] {
        ++missCounter_;
        return nullptr;
    }

    ++hitCounter_;
    moveToFront(it->second);
    return &it->second->value;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
const V* LRUCache<K, V, Hash, KeyEqual>::peek(const K& key) const {
    auto it = cache_.find(key);
    return it != cache_.end() ? &it->second->value : nullptr;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
bool LRUCache<K, V, Hash, KeyEqual>::contains(const K& key) const {
    return cache_.contains(key);
}


// ============================================================
// Section 7 — Capacity Management
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::rebuildPool(std::size_t newCapacity) {
    std::byte* newStorage = static_cast<std::byte*>(
        ::operator new(newCapacity * sizeof(Node<K, V>), std::align_val_t(alignof(Node<K, V>))));

    Link* oldHead = head_.next;
    Link* oldTail = &tail_;

    head_.next = &tail_;
    tail_.prev = &head_;

    std::size_t index = 0;
    Link* current  = oldHead;
    Link* prevLink = &head_;

    while (current != oldTail) {
        Node<K, V>* oldNode = static_cast<Node<K, V>*>(current);
        current = current->next;

        Node<K, V>* newNode = std::construct_at(
            reinterpret_cast<Node<K, V>*>(newStorage + index * sizeof(Node<K, V>)),
            std::move(oldNode->key), std::move(oldNode->value));

        newNode->prev  = prevLink;
        prevLink->next = newNode;
        prevLink       = newNode;

        cache_[newNode->key] = newNode;

        std::destroy_at(oldNode);
        ++index;
    }

    prevLink->next = &tail_;
    tail_.prev      = prevLink;

    ::operator delete(storage_, std::align_val_t(alignof(Node<K, V>)));
    storage_ = newStorage;

    freeHead_ = nullptr;
    for (std::size_t i = newCapacity; i-- > size_;) {
        FreeSlot* slot = std::construct_at(
            reinterpret_cast<FreeSlot*>(newStorage + i * sizeof(Node<K, V>)), FreeSlot{freeHead_});
        freeHead_ = slot;
    }

    capacity_ = newCapacity;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::resize(std::size_t newCapacity) {
    if (newCapacity == 0) {
        throw std::invalid_argument("LRUCache capacity must be greater than 0");
    }

    while (size_ > newCapacity) {
        evictLRU();
    }

    rebuildPool(newCapacity);
    cache_.reserve(newCapacity);
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::reserve(std::size_t count) {
    cache_.reserve(count);
}


// ============================================================
// Section 8 — Observers
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
std::vector<K> LRUCache<K, V, Hash, KeyEqual>::keys() const {
    std::vector<K> result;
    result.reserve(size_);

    for (Link* current = head_.next; current != &tail_; current = current->next) {
        result.push_back(static_cast<Node<K, V>*>(current)->key);
    }
    return result;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
const K* LRUCache<K, V, Hash, KeyEqual>::mostRecentKey() const noexcept {
    return head_.next == &tail_ ? nullptr : &static_cast<Node<K, V>*>(head_.next)->key;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
const K* LRUCache<K, V, Hash, KeyEqual>::leastRecentKey() const noexcept {
    return tail_.prev == &head_ ? nullptr : &static_cast<Node<K, V>*>(tail_.prev)->key;
}


// ============================================================
// Section 9 — Statistics
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::hitCount() const noexcept { return hitCounter_; }

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::missCount() const noexcept { return missCounter_; }

template<typename K, typename V, typename Hash, typename KeyEqual>
double LRUCache<K, V, Hash, KeyEqual>::hitRate() const noexcept {
    std::size_t total = hitCounter_ + missCounter_;
    return total == 0 ? 0.0 : (static_cast<double>(hitCounter_) / static_cast<double>(total)) * 100.0;
}

template<typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::resetStats() noexcept {
    hitCounter_  = 0;
    missCounter_ = 0;
}


// ============================================================
// Section 10 — Capacity Queries
// ============================================================

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::capacity() const noexcept { return capacity_; }

template<typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::size() const noexcept { return size_; }

template<typename K, typename V, typename Hash, typename KeyEqual>
bool LRUCache<K, V, Hash, KeyEqual>::empty() const noexcept { return size_ == 0; }

} // namespace CachePro