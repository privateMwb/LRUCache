// LRUCache external synchronization test suite.
//
// Coverage:
// - LRUCache intentionally provides no internal synchronization.
// - Concurrent access is therefore safe only when callers provide
//   external synchronization.
// - This suite verifies that multiple threads can safely access the
//   same LRUCache when protected by an external mutex.
// - Assertions verify that all concurrent operations complete and
//   that the cache remains within its configured capacity.

#include <CachePro/LRUCache.h>
#include <gtest/gtest.h>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

using namespace CachePro;

namespace {
constexpr int kThreadCount = 4;
constexpr int kOpsPerThread = 200;
constexpr int kCacheCapacity = 8;
} // namespace

// Verifies that concurrent callers can safely access the cache when
// protected by an external mutex.
TEST(ExternalSynchronization, SynchronizedConcurrentPuts) {
    LRUCache<int, int> cache(kCacheCapacity);
    std::mutex mutex;
    std::atomic<int> completed{0};

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int t = 0; t < kThreadCount; ++t) {
        threads.emplace_back([&, t] {
            for (int i = 0; i < kOpsPerThread; ++i) {
                std::lock_guard<std::mutex> lock(mutex);
                cache.put((t * kOpsPerThread) + i, i);
            }

            completed.fetch_add(1, std::memory_order_relaxed);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(completed.load(std::memory_order_relaxed), kThreadCount);
    EXPECT_LE(cache.size(), kCacheCapacity);
}

// Verifies that concurrent get() and put() operations are safe when
// protected by an external mutex.
TEST(ExternalSynchronization, SynchronizedConcurrentReadWrite) {
    LRUCache<int, int> cache(kCacheCapacity);
    std::mutex mutex;

    cache.put(1, 0);

    std::atomic<int> completed{0};

    std::thread writer([&] {
        for (int i = 0; i < kOpsPerThread; ++i) {
            std::lock_guard<std::mutex> lock(mutex);
            cache.put(1, i);
        }

        completed.fetch_add(1, std::memory_order_relaxed);
    });

    std::thread reader([&] {
        for (int i = 0; i < kOpsPerThread; ++i) {
            std::lock_guard<std::mutex> lock(mutex);
            (void)cache.get(1);
        }

        completed.fetch_add(1, std::memory_order_relaxed);
    });

    writer.join();
    reader.join();

    EXPECT_EQ(completed.load(std::memory_order_relaxed), 2);
    EXPECT_EQ(cache.size(), 1u);
    EXPECT_TRUE(cache.contains(1));
}

// Verifies that concurrent put() and erase() operations are safe when
// protected by an external mutex.
TEST(ExternalSynchronization, SynchronizedConcurrentPutErase) {
    LRUCache<int, int> cache(kCacheCapacity);
    std::mutex mutex;

    std::atomic<int> completed{0};

    std::thread writer([&] {
        for (int i = 0; i < kOpsPerThread; ++i) {
            std::lock_guard<std::mutex> lock(mutex);
            cache.put(i % kCacheCapacity, i);
        }

        completed.fetch_add(1, std::memory_order_relaxed);
    });

    std::thread eraser([&] {
        for (int i = 0; i < kOpsPerThread; ++i) {
            std::lock_guard<std::mutex> lock(mutex);
            (void)cache.erase(i % kCacheCapacity);
        }

        completed.fetch_add(1, std::memory_order_relaxed);
    });

    writer.join();
    eraser.join();

    EXPECT_EQ(completed.load(std::memory_order_relaxed), 2);
    EXPECT_LE(cache.size(), kCacheCapacity);
}
