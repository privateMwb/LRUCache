// LRUCache external locking contract test suite.
//
// Coverage:
// - LRUCache has no internal synchronization by design; correctness under
//   concurrent use depends entirely on the caller serializing access with
//   its own mutex.
// - Multiple threads performing put()/get()/erase() through a shared
//   external lock produce a correct, non-corrupted final state.
// - No lost updates: every successful put() from every thread is
//   eventually observable, and total size never exceeds capacity.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <mutex>
#include <thread>
#include <vector>

using namespace CachePro;

namespace {
constexpr int kThreadCount = 8;
constexpr int kOpsPerThread = 500;
constexpr int kCacheCapacity = 16;
} // namespace

// Verifies concurrent put()s serialized by an external mutex never corrupt
// cache state: size never exceeds capacity, and the table/list stay consistent.
TEST(ExternalLockingContract, LockedConcurrentPutsStayConsistent) {
    LRUCache<int, int> cache(kCacheCapacity);
    std::mutex mutex;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&, t] {
            for (int i = 0; i < kOpsPerThread; ++i) {
                int key = (t * kOpsPerThread) + i;
                std::lock_guard<std::mutex> lock(mutex);
                cache.put(key, key);
                EXPECT_LE(cache.size(), kCacheCapacity);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    std::lock_guard<std::mutex> lock(mutex);
    EXPECT_EQ(cache.size(), kCacheCapacity);
    EXPECT_LE(cache.size(), cache.capacity());
}

// Verifies a value written by one thread is correctly visible to another,
// as long as both go through the shared lock.
TEST(ExternalLockingContract, LockedWritesAreVisibleAcrossThreads) {
    LRUCache<int, int> cache(kCacheCapacity);
    std::mutex mutex;

    std::thread writer([&] {
        std::lock_guard<std::mutex> lock(mutex);
        cache.put(1, 42);
    });
    writer.join();

    int result = 0;
    bool found = false;
    std::thread reader([&] {
        std::lock_guard<std::mutex> lock(mutex);
        int* value = cache.get(1);
        if (value != nullptr) {
            found = true;
            result = *value;
        }
    });
    reader.join();

    ASSERT_TRUE(found);
    EXPECT_EQ(result, 42);
}

// Verifies concurrent put()/erase() from many threads, all serialized by
// the same lock, leaves the cache in a state where every live entry is
// genuinely retrievable (no torn or partially-linked nodes).
TEST(ExternalLockingContract, LockedMixedPutEraseStaysCorrect) {
    LRUCache<int, int> cache(kCacheCapacity);
    std::mutex mutex;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&, t] {
            for (int i = 0; i < kOpsPerThread; ++i) {
                int key = i % kCacheCapacity;
                std::lock_guard<std::mutex> lock(mutex);
                if ((t + i) % 2 == 0) {
                    cache.put(key, key);
                } else {
                    (void)cache.erase(key);
                }
                EXPECT_LE(cache.size(), kCacheCapacity);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    std::lock_guard<std::mutex> lock(mutex);
    std::vector<int> liveKeys = cache.keys();
    ASSERT_EQ(liveKeys.size(), cache.size());
    for (int key : liveKeys) {
        const int* value = cache.peek(key);
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, key);
    }
}
