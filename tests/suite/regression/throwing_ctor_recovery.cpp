// LRUCache throwing constructor recovery regression test suite.
//
// Coverage:
// - When K's or V's constructor throws inside acquireNode()/
//   acquireNodeEmplace(), the pool slot taken off the free list must be
//   returned to it before the exception propagates — not leaked.
// - After such a throw, the cache's size(), contents, and free list must
//   be exactly as they were before the failed call, and subsequent
//   put()/emplace() calls must still succeed correctly.

#include <support/framework.h>

#include <stdexcept>

using namespace CachePro;

namespace {

// A value type whose *move* construction can be made to throw on demand.
// put(K, V&&) constructs the V temporary successfully outside the cache,
// then move-constructs it into the pool slot inside acquireNode() — it's
// that internal move-construction that must trigger the recovery path,
// not construction of the caller's temporary. emplace() constructs V
// in place directly from its constructor instead, so a throwing
// single-argument constructor is used to hit the same path there.
struct ThrowingValue {
    static bool shouldThrow;
    int data;

    explicit ThrowingValue(int d) : data(d) {
        if (shouldThrow) {
            throw std::runtime_error("boom");
        }
    }

    ThrowingValue(const ThrowingValue& other) : data(other.data) {}

    ThrowingValue(ThrowingValue&& other) noexcept(false) : data(other.data) {
        if (shouldThrow) {
            throw std::runtime_error("boom");
        }
    }

    ThrowingValue& operator=(const ThrowingValue& other) {
        data = other.data;
        return *this;
    }

    ThrowingValue& operator=(ThrowingValue&& other) noexcept {
        data = other.data;
        return *this;
    }
};

bool ThrowingValue::shouldThrow = false;

} // namespace

// Verifies a throwing move constructor during put()'s internal
// acquireNode() call restores the free list, leaving size() and existing
// contents unchanged.
static void put_ctor_throw_restores_free_list() {
    ThrowingValue::shouldThrow = false;
    LRUCache<int, ThrowingValue> cache(2);
    cache.put(1, ThrowingValue(10));
    CHK(cache.size() == 1);

    // Constructing this temporary succeeds (shouldThrow is still false);
    // the throw is enabled just before the call so it fires during the
    // internal move-construction inside acquireNode(), not here.
    ThrowingValue incoming(20);
    ThrowingValue::shouldThrow = true;
    CHK_THROWS(cache.put(2, std::move(incoming)), std::runtime_error);
    ThrowingValue::shouldThrow = false;

    CHK(cache.size() == 1);
    CHK(cache.contains(1));

    // A subsequent successful put() must still work — proof the free list
    // wasn't corrupted by the earlier throw.
    cache.put(3, ThrowingValue(30));
    CHK(cache.size() == 2);
    CHK(cache.contains(3));
}

// Verifies a throwing value constructor during emplace() restores the
// free list the same way.
static void emplace_ctor_throw_restores_free_list() {
    ThrowingValue::shouldThrow = false;
    LRUCache<int, ThrowingValue> cache(2);
    cache.emplace(1, 10);
    CHK(cache.size() == 1);

    ThrowingValue::shouldThrow = true;
    CHK_THROWS(cache.emplace(2, 20), std::runtime_error);
    ThrowingValue::shouldThrow = false;

    CHK(cache.size() == 1);
    CHK(cache.contains(1));

    cache.emplace(3, 30);
    CHK(cache.size() == 2);
    CHK(cache.contains(3));
}

// Verifies repeated throw/succeed cycles never leak or double-free pool slots.
static void repeated_throw_cycles_keep_pool_consistent() {
    LRUCache<int, ThrowingValue> cache(3);
    cache.emplace(0, 0);

    for (int i = 1; i <= 50; ++i) {
        ThrowingValue::shouldThrow = true;
        CHK_THROWS(cache.emplace(1000 + i, i), std::runtime_error);

        ThrowingValue::shouldThrow = false;
        cache.emplace(i, i); // must always succeed if the free list is intact
        CHK(cache.contains(i));
    }
}

// Executes all throwing constructor recovery test cases.
static void run_tests() {
    RUN(put_ctor_throw_restores_free_list);
    RUN(emplace_ctor_throw_restores_free_list);
    RUN(repeated_throw_cycles_keep_pool_consistent);
}

REGISTER_TEST_SUITE();
