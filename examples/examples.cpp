// LRUCache Examples
// Demonstrates real-world usage of the LRU cache container:
//
// - basicUsage       : simple put/get operations
// - cacheMiss        : handling cache misses with a fallback
// - dnsCache         : mapping hostnames to IP addresses
// - sessionStore     : mapping session tokens to user data
// - memoization      : caching expensive function results
// - peekVsGet        : difference in LRU ordering between peek and get
// - evictionPolicy   : observing LRU eviction at capacity
// - moveOwnership    : transferring cache ownership via move semantics
// - cacheStatistics  : tracking hit rate over a series of accesses

#include <iostream>
#include <string>
#include <functional>

#include "LRUCache.h"

// Basic Usage
// Demonstrates simple put and get operations.
void basicUsage() {
    std::cout << "Basic Usage\n";
    std::cout << "-------------------\n";

    LRUCache<int, std::string> lru(3);

    lru.put(1, "Apple");
    lru.put(2, "Banana");
    lru.put(3, "Mango");

    auto* item = lru.get(1);

    if (item) {
        std::cout << "Got key 1: " << *item << "\n";
    }

    item = lru.get(4);

    if (!item) {
        std::cout << "Key 4 not found\n";
    }

    std::cout << "Size: " << lru.size() << " / " << lru.capacity() << "\n";

    std::cout << "\n";
}

// Cache Miss
// Demonstrates handling a cache miss by falling back to a simulated database.
void cacheMiss() {
    std::cout << "Cache Miss\n";
    std::cout << "-------------------\n";

    LRUCache<int, std::string> lru(5);

    auto fetch = [&](int id) -> std::string {
        auto* cached = lru.get(id);

        if (cached) {
            std::cout << "Cache hit  - id " << id << ": " << *cached << "\n";
            return *cached;
        }

        std::cout << "Cache miss - fetching id " << id << " from database...\n";

        std::string result = "User_" + std::to_string(id);

        lru.put(id, result);

        std::cout << "Cached id " << id << ": " << result << "\n";

        return result;
    };

    fetch(1);
    fetch(2);
    fetch(1);
    fetch(3);
    fetch(2);

    std::cout << "\n";
}

// DNS Cache
// Demonstrates caching hostname to IP address mappings.
void dnsCache() {
    std::cout << "DNS Cache\n";
    std::cout << "-------------------\n";

    LRUCache<std::string, std::string> lru(4);

    auto resolve = [&](const std::string& hostname) -> std::string {
        auto* cached = lru.get(hostname);

        if (cached) {
            std::cout << "Cache hit  - " << hostname << " -> " << *cached << "\n";
            return *cached;
        }

        std::cout << "Resolving " << hostname << "...\n";

        std::string ip = "192.168.0." + std::to_string(lru.size() + 1);

        lru.put(hostname, ip);

        std::cout << "Resolved   - " << hostname << " -> " << ip << "\n";

        return ip;
    };

    resolve("google.com");
    resolve("github.com");
    resolve("anthropic.com");
    resolve("google.com");
    resolve("openai.com");
    resolve("github.com");

    std::cout << "Most recent: " << *lru.mostRecentKey() << "\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << "\n";

    std::cout << "\n";
}

// Session Store
// Demonstrates caching session tokens mapped to usernames.
void sessionStore() {
    std::cout << "Session Store\n";
    std::cout << "-------------------\n";

    LRUCache<std::string, std::string> lru(3);

    lru.put("tok_a1b2", "alice");
    lru.put("tok_c3d4", "bob");
    lru.put("tok_e5f6", "carol");

    auto authenticate = [&](const std::string& token) {
        auto* user = lru.get(token);

        if (user) {
            std::cout << "Authenticated - token " << token << " -> " << *user << "\n";
        } else {
            std::cout << "Rejected      - token " << token << " not found\n";
        }
    };

    authenticate("tok_a1b2");
    authenticate("tok_e5f6");
    authenticate("tok_x9y0");

    lru.put("tok_g7h8", "dave");

    std::cout << "Evicted least recently used session\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << "\n";

    std::cout << "\n";
}

// Memoization
// Demonstrates caching expensive function results using factorial as an example.
void memoization() {
    std::cout << "Memoization\n";
    std::cout << "-------------------\n";

    LRUCache<int, long long> lru(10);

    std::function<long long(int)> factorial = [&](int n) -> long long {
        if (n <= 1) return 1;

        auto* cached = lru.get(n);

        if (cached) {
            std::cout << "Cache hit  - " << n << "! = " << *cached << "\n";
            return *cached;
        }

        std::cout << "Computing  - " << n << "!\n";

        long long result = n * factorial(n - 1);

        lru.put(n, result);

        return result;
    };

    std::cout << "5! = " << factorial(5) << "\n";
    std::cout << "---\n";
    std::cout << "7! = " << factorial(7) << "\n";
    std::cout << "---\n";
    std::cout << "5! = " << factorial(5) << "\n";

    std::cout << "\n";
}

// Peek vs Get
// Demonstrates that peek() does not affect LRU ordering while get() does.
void peekVsGet() {
    std::cout << "Peek vs Get\n";
    std::cout << "-------------------\n";

    LRUCache<int, std::string> lru(3);

    lru.put(1, "First");
    lru.put(2, "Second");
    lru.put(3, "Third");

    std::cout << "Most recent : " << *lru.mostRecentKey() << "\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << "\n";

    std::cout << "--- Peeking at key 1 ---\n";

    auto* peeked = lru.peek(1);

    std::cout << "Peeked key 1: " << *peeked << "\n";
    std::cout << "Most recent : " << *lru.mostRecentKey() << " (unchanged)\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << " (unchanged)\n";

    std::cout << "--- Getting key 1 ---\n";

    auto* gotten = lru.get(1);

    std::cout << "Got key 1   : " << *gotten << "\n";
    std::cout << "Most recent : " << *lru.mostRecentKey() << " (promoted)\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << " (shifted)\n";

    std::cout << "\n";
}

// Eviction Policy
// Demonstrates LRU eviction when the cache reaches capacity.
void evictionPolicy() {
    std::cout << "Eviction Policy\n";
    std::cout << "-------------------\n";

    LRUCache<int, std::string> lru(3);

    lru.put(1, "One");
    lru.put(2, "Two");
    lru.put(3, "Three");

    std::cout << "Cache full  - size: " << lru.size() << " / " << lru.capacity() << "\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << "\n";

    (void)lru.get(1);
    (void)lru.get(2);

    std::cout << "Accessed keys 1 and 2\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << " (will be evicted next)\n";

    lru.put(4, "Four");

    std::cout << "Inserted key 4 - key 3 evicted\n";
    std::cout << "Contains key 3: " << (lru.contains(3) ? "yes" : "no") << "\n";
    std::cout << "Contains key 4: " << (lru.contains(4) ? "yes" : "no") << "\n";
    std::cout << "Least recent: " << *lru.leastRecentKey() << "\n";

    std::cout << "\n";
}

// Move Ownership
// Demonstrates transferring cache ownership via move construction and assignment.
void moveOwnership() {
    std::cout << "Move Ownership\n";
    std::cout << "-------------------\n";

    LRUCache<int, std::string> original(5);

    original.put(1, "One");
    original.put(2, "Two");
    original.put(3, "Three");

    std::cout << "Original - size: " << original.size() << " capacity: " << original.capacity() << "\n";

    LRUCache<int, std::string> moved(std::move(original));

    std::cout << "After move construction:\n";
    std::cout << "Original - size: " << original.size() << " capacity: " << original.capacity() << "\n";
    std::cout << "Moved    - size: " << moved.size()    << " capacity: " << moved.capacity()    << "\n";

    auto* item = moved.get(2);

    if (item) {
        std::cout << "Moved cache still has key 2: " << *item << "\n";
    }

    LRUCache<int, std::string> assigned(2);

    assigned.put(9, "Nine");
    assigned = std::move(moved);

    std::cout << "After move assignment:\n";
    std::cout << "Assigned - size: " << assigned.size() << " capacity: " << assigned.capacity() << "\n";

    std::cout << "\n";
}

// Cache Statistics
// Demonstrates hit rate tracking over a series of accesses.
void cacheStatistics() {
    std::cout << "Cache Statistics\n";
    std::cout << "-------------------\n";

    LRUCache<int, std::string> lru(5);

    lru.put(1, "One");
    lru.put(2, "Two");
    lru.put(3, "Three");

    (void)lru.get(1);
    (void)lru.get(2);
    (void)lru.get(3);
    (void)lru.get(4);
    (void)lru.get(5);

    std::cout << "Hits  : " << lru.hitCount()  << "\n";
    std::cout << "Misses: " << lru.missCount() << "\n";
    std::cout << "Rate  : " << lru.hitRate()   << "%\n";

    lru.resetStats();

    std::cout << "After reset:\n";
    std::cout << "Hits  : " << lru.hitCount()  << "\n";
    std::cout << "Misses: " << lru.missCount() << "\n";
    std::cout << "Rate  : " << lru.hitRate()   << "%\n";

    std::cout << "\n";
}

int main() {
    basicUsage();
    cacheMiss();
    dnsCache();
    sessionStore();
    memoization();
    peekVsGet();
    evictionPolicy();
    moveOwnership();
    cacheStatistics();

    return 0;
}

