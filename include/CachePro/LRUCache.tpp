/**
 * @file LRUCache.tpp
 * @brief LRUCache template implementation.
 *
 * Contains the implementation of LRUCache template member
 * functions and internal implementation details.
 */

// ============================================================
// Template implementation for CachePro::LRUCache.
// ============================================================
//
//  Sections:
//   1. Pool Management
//   2. Node Management
//   3. LRU List Management
//   4. Table Management
//   5. Constructors & Destructor
//   6. Modifiers
//   7. Lookup
//   8. Capacity Management
//   9. Observers
//  10. Statistics
//  11. Capacity Queries
//
// ============================================================

// clang-format off
#include <algorithm> // std::max
#include <memory>    // std::construct_at, std::destroy_at
#include <new>       // ::operator new/delete, std::align_val_t
#include <stdexcept> // std::invalid_argument
#include <utility>   // std::move, std::forward
#include <limits>    // std::numeric_limits
// clang-format on

namespace CachePro {

// ============================================================
//  Section 1 — Pool Management
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::allocatePool(std::size_t count) {
    storage_ = static_cast<std::byte*>(
        ::operator new(count * sizeof(Node<K, V>), std::align_val_t(alignof(Node<K, V>))));

    freeHead_ = nullptr;
    for (std::size_t i = count; i-- > 0;) {
        FreeSlot* slot = std::construct_at(
            reinterpret_cast<FreeSlot*>(storage_ + i * sizeof(Node<K, V>)), FreeSlot{freeHead_});
        freeHead_ = slot;
    }
    reservedCapacity_ = count;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::freePool() noexcept {
    ::operator delete(storage_, std::align_val_t(alignof(Node<K, V>)));
    storage_ = nullptr;
    freeHead_ = nullptr;
}

// ============================================================
//  Section 2 — Node Management
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
template <typename K2, typename V2>
Node<K, V>* LRUCache<K, V, Hash, KeyEqual>::acquireNode(K2&& key, V2&& value) {
    FreeSlot* slot = freeHead_;
    FreeSlot* nextFree = slot->next; // must read before this memory is overwritten below
    std::destroy_at(slot);

    Node<K, V>* node;
    try {
        node = std::construct_at(reinterpret_cast<Node<K, V>*>(slot), std::forward<K2>(key),
                                 std::forward<V2>(value));
    } catch (...) {
        // K's or V's constructor threw. Node's own construction already
        // unwound (any subobject that did finish construction is
        // destroyed automatically), but freeHead_ (the member) was never
        // advanced, so restoring a FreeSlot here is enough to put the pool
        // back exactly as it was before this call.
        std::construct_at(slot, FreeSlot{nextFree});
        throw;
    }

    freeHead_ = nextFree;
    ++size_;
    return node;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
template <typename K2, typename... Args>
Node<K, V>* LRUCache<K, V, Hash, KeyEqual>::acquireNodeEmplace(K2&& key, Args&&... args) {
    FreeSlot* slot = freeHead_;
    FreeSlot* nextFree = slot->next;
    std::destroy_at(slot);

    Node<K, V>* node;
    try {
        node = std::construct_at(reinterpret_cast<Node<K, V>*>(slot), std::forward<K2>(key),
                                 std::in_place, std::forward<Args>(args)...);
    } catch (...) {
        std::construct_at(slot, FreeSlot{nextFree});
        throw;
    }

    freeHead_ = nextFree;
    ++size_;
    return node;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::releaseNode(Node<K, V>* node) noexcept {
    std::destroy_at(node);
    FreeSlot* slot = std::construct_at(reinterpret_cast<FreeSlot*>(node), FreeSlot{freeHead_});
    freeHead_ = slot;
    --size_;
}

// ============================================================
//  Section 3 — LRU List Management
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::insertFront(Node<K, V>* node) noexcept {
    Link* n = node;
    n->next = head_.next;
    n->prev = &head_;
    head_.next->prev = n;
    head_.next = n;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::removeNode(Node<K, V>* node) noexcept {
    Link* n = node;
    n->prev->next = n->next;
    n->next->prev = n->prev;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::moveToFront(Node<K, V>* node) noexcept {
    if (head_.next == node) [[unlikely]] {
        return;
    }
    removeNode(node);
    insertFront(node);
}

template <typename K, typename V, typename Hash, typename KeyEqual>
Node<K, V>* LRUCache<K, V, Hash, KeyEqual>::popBack() noexcept {
    Node<K, V>* node = static_cast<Node<K, V>*>(tail_.prev);
    removeNode(node);
    return node;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::evictLRU() noexcept {
    Node<K, V>* lru = popBack();
    eraseSlot(lru->slot); // direct — no re-hash, no re-probe
    releaseNode(lru);
}

// ============================================================
//  Section 4 — Table Management
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::tableSizeFor(std::size_t capacity) noexcept {
    std::size_t minSlots = capacity * 2; // bound load factor at <= 0.5
    std::size_t power = 2;               // guarantee >= 1 permanently-reachable empty slot
    while (power < minSlots) {
        power <<= 1;
    }
    return power;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::allocateTable(std::size_t count) {
    table_ = new Slot[count]; // default member initializers zero every slot (hash=0, node=nullptr)
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::freeTable() noexcept {
    delete[] table_;
    table_ = nullptr;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::rebuildTable(Slot* newTable, std::size_t newTableCapacity) {
    Slot* oldTable = table_;

    table_ = newTable;
    tableCapacity_ = newTableCapacity;
    tableMask_ = newTableCapacity - 1;

    for (Link* current = head_.next; current != &tail_; current = current->next) {
        Node<K, V>* node = static_cast<Node<K, V>*>(current);
        insertSlot(computeHash(node->key), node);
    }

    delete[] oldTable;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::mixHash(std::size_t h) noexcept {
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= h >> 33;
    return h;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::computeHash(const K& key) const
    noexcept(noexcept(hasher_(key))) {
    return mixHash(hasher_(key));
}

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::findSlot(const K& key,
                                                     std::size_t hash) const noexcept {
    std::size_t idx = hash & tableMask_;

    while (table_[idx].node != nullptr) {
        if (table_[idx].hash == hash && keyEqual_(table_[idx].node->key, key)) [[unlikely]] {
            return idx;
        }
        idx = (idx + 1) & tableMask_;
    }
    return tableCapacity_; // not found
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::insertSlot(std::size_t hash, Node<K, V>* node) noexcept {
    std::size_t idx = hash & tableMask_;

    while (table_[idx].node != nullptr) {
        idx = (idx + 1) & tableMask_;
    }

    table_[idx].hash = hash;
    table_[idx].node = node;
    node->slot = idx;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::eraseSlot(std::size_t index) noexcept {
    table_[index].node = nullptr;

    std::size_t hole = index;
    std::size_t probe = index;

    for (;;) {
        probe = (probe + 1) & tableMask_;
        Node<K, V>* candidate = table_[probe].node;
        if (candidate == nullptr) {
            return; // ran off the end of this cluster — done
        }

        std::size_t home = table_[probe].hash & tableMask_;
        std::size_t distToHole = (hole - home) & tableMask_;
        std::size_t distToProbe = (probe - home) & tableMask_;

        // Move `candidate` back into `hole` only if doing so keeps it
        // reachable from its own home slot (i.e. `hole` lies within its
        // current probe run). Otherwise it must stay put and we keep
        // scanning — a later entry in the cluster may still be movable.
        if (distToHole < distToProbe) {
            table_[hole] = table_[probe];
            table_[hole].node->slot = hole;
            table_[probe].node = nullptr;
            hole = probe;
        }
    }
}

// ============================================================
//  Section 5 — Constructors & Destructor
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>::LRUCache(std::size_t capacity)
    : capacity_(capacity), size_(0), storage_(nullptr), freeHead_(nullptr), reservedCapacity_(0),
      head_{nullptr, nullptr}, tail_{nullptr, nullptr}, table_(nullptr), tableCapacity_(0),
      tableMask_(0), hitCounter_(0), missCounter_(0) {
    if (capacity == 0) {
        throw std::invalid_argument("LRUCache capacity must be greater than 0");
    }

    head_.next = &tail_;
    tail_.prev = &head_;

    allocatePool(capacity_);

    try {
        tableCapacity_ = tableSizeFor(capacity_);
        tableMask_ = tableCapacity_ - 1;
        allocateTable(tableCapacity_);
    } catch (...) {
        freePool();
        throw;
    }
}

template <typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>::~LRUCache() {
    clear();
    freePool();
    freeTable();
}

template <typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>::LRUCache(LRUCache&& other) noexcept
    : capacity_(other.capacity_), size_(other.size_), storage_(other.storage_),
      freeHead_(other.freeHead_), reservedCapacity_(other.reservedCapacity_),
      head_{nullptr, nullptr}, tail_{nullptr, nullptr}, table_(other.table_),
      tableCapacity_(other.tableCapacity_), tableMask_(other.tableMask_),
      hasher_(std::move(other.hasher_)), keyEqual_(std::move(other.keyEqual_)),
      hitCounter_(other.hitCounter_), missCounter_(other.missCounter_) {
    if (other.head_.next == &other.tail_) {
        head_.next = &tail_;
        tail_.prev = &head_;
    } else {
        head_.next = other.head_.next;
        tail_.prev = other.tail_.prev;
        head_.next->prev = &head_;
        tail_.prev->next = &tail_;
    }

    other.capacity_ = 0;
    other.size_ = 0;
    other.storage_ = nullptr;
    other.freeHead_ = nullptr;
    other.reservedCapacity_ = 0;
    other.head_.next = &other.tail_;
    other.tail_.prev = &other.head_;
    other.table_ = nullptr;
    other.tableCapacity_ = 0;
    other.tableMask_ = 0;
    other.hitCounter_ = 0;
    other.missCounter_ = 0;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
LRUCache<K, V, Hash, KeyEqual>&
LRUCache<K, V, Hash, KeyEqual>::operator=(LRUCache&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    clear();
    freePool();
    freeTable();

    capacity_ = other.capacity_;
    size_ = other.size_;
    storage_ = other.storage_;
    freeHead_ = other.freeHead_;
    reservedCapacity_ = other.reservedCapacity_;
    table_ = other.table_;
    tableCapacity_ = other.tableCapacity_;
    tableMask_ = other.tableMask_;
    hasher_ = std::move(other.hasher_);
    keyEqual_ = std::move(other.keyEqual_);
    hitCounter_ = other.hitCounter_;
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

    other.capacity_ = 0;
    other.size_ = 0;
    other.storage_ = nullptr;
    other.freeHead_ = nullptr;
    other.reservedCapacity_ = 0;
    other.head_.next = &other.tail_;
    other.tail_.prev = &other.head_;
    other.table_ = nullptr;
    other.tableCapacity_ = 0;
    other.tableMask_ = 0;
    other.hitCounter_ = 0;
    other.missCounter_ = 0;

    return *this;
}

// ============================================================
//  Section 6 — Modifiers
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
template <typename K2, typename V2>
void LRUCache<K, V, Hash, KeyEqual>::putImpl(K2&& key, V2&& value) {
    if (capacity_ == 0 || storage_ == nullptr || table_ == nullptr) {
        throw std::invalid_argument("Cannot call put() on a moved-from LRUCache");
    }

    std::size_t hash = computeHash(key);
    std::size_t idx = findSlot(key, hash);

    if (idx != tableCapacity_) {
        table_[idx].node->value = std::forward<V2>(value);
        moveToFront(table_[idx].node);
        return;
    }

    if (size_ == capacity_) [[unlikely]] {
        evictLRU();
    }

    // Node is fully constructed — including K's and V's constructors,
    // which may throw — before anything touches the table or list. If
    // acquireNode() throws, the cache is left exactly as it was.
    Node<K, V>* node = acquireNode(std::forward<K2>(key), std::forward<V2>(value));
    insertSlot(hash, node);
    insertFront(node);
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(const K& key, const V& value) {
    putImpl(key, value);
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(const K& key, V&& value) {
    putImpl(key, std::move(value));
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(K&& key, const V& value) {
    putImpl(std::move(key), value);
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::put(K&& key, V&& value) {
    putImpl(std::move(key), std::move(value));
}

template <typename K, typename V, typename Hash, typename KeyEqual>
template <typename... Args>
V& LRUCache<K, V, Hash, KeyEqual>::emplace(const K& key, Args&&... args) {
    std::size_t hash = computeHash(key);
    std::size_t idx = findSlot(key, hash); // single probe — was find() + emplace() (two) before

    if (idx != tableCapacity_) {
        table_[idx].node->value = V(std::forward<Args>(args)...);
        moveToFront(table_[idx].node);
        return table_[idx].node->value;
    }

    if (size_ == capacity_) [[unlikely]] {
        evictLRU();
    }

    // Forwards args straight into V's constructor — no intermediate V
    // temporary, unlike the update path above (which must build one to
    // replace an existing live value).
    Node<K, V>* node = acquireNodeEmplace(key, std::forward<Args>(args)...);
    insertSlot(hash, node);
    insertFront(node);
    return node->value;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
bool LRUCache<K, V, Hash, KeyEqual>::erase(const K& key) {
    std::size_t hash = computeHash(key);
    std::size_t idx = findSlot(key, hash);
    if (idx == tableCapacity_) {
        return false;
    }

    Node<K, V>* node = table_[idx].node;
    removeNode(node);
    eraseSlot(idx);
    releaseNode(node);
    return true;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::clear() noexcept {
    Link* current = head_.next;

    while (current != &tail_) {
        Node<K, V>* node = static_cast<Node<K, V>*>(current);
        current = current->next;
        releaseNode(node);
    }

    head_.next = &tail_;
    tail_.prev = &head_;

    for (std::size_t i = 0; i < tableCapacity_; ++i) {
        table_[i] = Slot{};
    }
}

// ============================================================
//  Section 7 — Lookup
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
V* LRUCache<K, V, Hash, KeyEqual>::get(const K& key) {
    std::size_t hash = computeHash(key);
    std::size_t idx = findSlot(key, hash);

    if (idx == tableCapacity_) [[unlikely]] {
        ++missCounter_;
        return nullptr;
    }

    ++hitCounter_;
    Node<K, V>* node = table_[idx].node;
    moveToFront(node);
    return &node->value;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
const V* LRUCache<K, V, Hash, KeyEqual>::peek(const K& key) const {
    std::size_t idx = findSlot(key, computeHash(key));
    return idx != tableCapacity_ ? &table_[idx].node->value : nullptr;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
bool LRUCache<K, V, Hash, KeyEqual>::contains(const K& key) const {
    return findSlot(key, computeHash(key)) != tableCapacity_;
}

// ============================================================
//  Section 8 — Capacity Management
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::rebuildPool(std::size_t newReserved) {
    std::byte* newStorage = static_cast<std::byte*>(
        ::operator new(newReserved * sizeof(Node<K, V>), std::align_val_t(alignof(Node<K, V>))));

    Link* oldHead = head_.next;
    Link* oldTail = &tail_;

    head_.next = &tail_;
    tail_.prev = &head_;

    std::size_t index = 0;
    Link* current = oldHead;
    Link* prevLink = &head_;

    while (current != oldTail) {
        Node<K, V>* oldNode = static_cast<Node<K, V>*>(current);
        current = current->next;

        Node<K, V>* newNode = std::construct_at(
            reinterpret_cast<Node<K, V>*>(newStorage + index * sizeof(Node<K, V>)),
            std::move(oldNode->key), std::move(oldNode->value));

        newNode->prev = prevLink;
        prevLink->next = newNode;
        prevLink = newNode;

        std::destroy_at(oldNode);
        ++index;
    }

    prevLink->next = &tail_;
    tail_.prev = prevLink;

    ::operator delete(storage_, std::align_val_t(alignof(Node<K, V>)));
    storage_ = newStorage;

    freeHead_ = nullptr;
    for (std::size_t i = newReserved; i-- > size_;) {
        FreeSlot* slot = std::construct_at(
            reinterpret_cast<FreeSlot*>(newStorage + i * sizeof(Node<K, V>)), FreeSlot{freeHead_});
        freeHead_ = slot;
    }

    reservedCapacity_ = newReserved;
    // Note: table_ still points at the old (now stale) node addresses via
    // each slot's Node* — caller must follow this with rebuildTable().
    // capacity_ is the caller's responsibility now, not this function's —
    // this only tracks physical storage, not the logical bound.
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::shrink(std::size_t newCapacity) {
    if (newCapacity == 0) {
        throw std::invalid_argument("LRUCache capacity must be greater than 0");
    }
    if (newCapacity > capacity_) {
        throw std::invalid_argument("shrink() cannot grow capacity — use resize()");
    }

    while (size_ > newCapacity) {
        evictLRU();
    }

    capacity_ = newCapacity;
    // Pool + table are left at their current (larger) physical size —
    // this only lowers the logical bound. put() evicts once
    // size_ == capacity_, so the now-excess free-list slots and table
    // headroom are simply never touched again. Call resize() instead
    // if the memory needs to be physically reclaimed, or
    // shrink_to_fit() to reclaim down to the current size.
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::shrink_to_fit() {
    if (size_ == 0 || size_ == capacity_) {
        return; // nothing to reclaim
    }

    std::size_t newTableCapacity = tableSizeFor(size_);
    Slot* newTable = new Slot[newTableCapacity]; // may throw -- nothing committed yet

    rebuildPool(size_);                       // must run first — moves node addresses
    rebuildTable(newTable, newTableCapacity); // re-derives every slot from the now-current list

    capacity_ = size_;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::resize(std::size_t newCapacity) {
    if (newCapacity == 0) {
        throw std::invalid_argument("LRUCache capacity must be greater than 0");
    }

    if (newCapacity > std::numeric_limits<std::size_t>::max() / sizeof(Node<K, V>)) {
        throw std::bad_alloc();
    }

    while (size_ > newCapacity) {
        evictLRU();
    }

    std::size_t newTableCapacity = tableSizeFor(newCapacity);

    // Lazy path: physical pool + table are already big enough — just
    // move the logical bound, no reallocation. Mirrors
    // std::unordered_map::reserve()'s no-op-when-sufficient behavior.
    if (newCapacity <= reservedCapacity_ && newTableCapacity <= tableCapacity_) {
        capacity_ = newCapacity;
        return;
    }

    // Real growth needed — pad the reservation (1.5x) so the next
    // several incremental resize() calls hit the lazy path above
    // instead of reallocating on every single call.
    std::size_t newReserved = std::max(newCapacity, reservedCapacity_ + reservedCapacity_ / 2 + 1);
    std::size_t newReservedTableCapacity = tableSizeFor(newReserved);

    Slot* newTable = new Slot[newReservedTableCapacity]; // may throw -- nothing committed yet

    rebuildPool(newReserved); // must run first — moves node addresses
    rebuildTable(newTable,
                 newReservedTableCapacity); // re-derives every slot from the now-current list

    capacity_ = newCapacity;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::reserve(std::size_t count) {
    if (count <= reservedCapacity_) {
        return; // already enough physical pool room
    }

    // rebuildPool() always relocates every live node to new addresses,
    // so rebuildTable() must always follow to re-point the table's
    // Node* pointers — even if the table's own capacity wouldn't
    // otherwise need to change. There is no valid "pool grows, table
    // stays" path.
    std::size_t newTableCapacity = tableSizeFor(count);
    Slot* newTable = new Slot[newTableCapacity]; // may throw -- nothing committed yet

    rebuildPool(count);                       // must run first — moves node addresses
    rebuildTable(newTable, newTableCapacity); // re-derives every slot from the now-current list
    // capacity_ (the logical bound) is intentionally untouched — this
    // only pre-grows physical storage so a later resize() up to
    // `count` hits resize()'s lazy path instead of reallocating.
}

// ============================================================
//  Section 9 — Observers
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
std::vector<K> LRUCache<K, V, Hash, KeyEqual>::keys() const {
    std::vector<K> result;
    result.reserve(size_);

    for (Link* current = head_.next; current != &tail_; current = current->next) {
        result.push_back(static_cast<Node<K, V>*>(current)->key);
    }
    return result;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
const K* LRUCache<K, V, Hash, KeyEqual>::mostRecentKey() const noexcept {
    return head_.next == &tail_ ? nullptr : &static_cast<Node<K, V>*>(head_.next)->key;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
const K* LRUCache<K, V, Hash, KeyEqual>::leastRecentKey() const noexcept {
    return tail_.prev == &head_ ? nullptr : &static_cast<Node<K, V>*>(tail_.prev)->key;
}

// ============================================================
//  Section 10 — Statistics
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::hitCount() const noexcept {
    return hitCounter_;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::missCount() const noexcept {
    return missCounter_;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
double LRUCache<K, V, Hash, KeyEqual>::hitRate() const noexcept {
    std::size_t total = hitCounter_ + missCounter_;
    return total == 0 ? 0.0
                      : (static_cast<double>(hitCounter_) / static_cast<double>(total)) * 100.0;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
void LRUCache<K, V, Hash, KeyEqual>::resetStats() noexcept {
    hitCounter_ = 0;
    missCounter_ = 0;
}

// ============================================================
//  Section 11 — Capacity Queries
// ============================================================

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::capacity() const noexcept {
    return capacity_;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
std::size_t LRUCache<K, V, Hash, KeyEqual>::size() const noexcept {
    return size_;
}

template <typename K, typename V, typename Hash, typename KeyEqual>
bool LRUCache<K, V, Hash, KeyEqual>::empty() const noexcept {
    return size_ == 0;
}

} // namespace CachePro

/// @brief Short alias so this library can be used as `rain::LRUCache`, while
/// its true namespace (and all internal diagnostics) remains `CachePro`.
/// See Node.h for the same alias applied to `rain::Node`.
namespace rain = CachePro;