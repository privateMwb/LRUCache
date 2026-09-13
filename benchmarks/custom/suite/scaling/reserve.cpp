// LRUCache Reserve Benchmark Suite
// Measures LRUCache reserve() performance against stdLRU.
//
// LRUCache's table is sized once for `capacity_` at construction and
// never grows on its own, so reserve() is a documented no-op — kept
// only for API compatibility with map-backed cache implementations.
// stdLRU's reserve() forwards to the real
// std::unordered_map::reserve(). The near-zero-cost result on the
// LRUCache side is the point of this comparison, not a flaw in it.
//
// Covers:
// - reserve() on an already-constructed cache

#include <support/framework.h>

using namespace CachePro;
using bench::stdLRU;

namespace {
constexpr std::size_t kCapacity = 1'000;
constexpr std::size_t kReserveCount = 1'000;
} // namespace

// Measures reserve() on a cache already sized for kCapacity entries.
static void bench_reserve() {
    LRUCache<int, int> cSrc(kCapacity);
    stdLRU<int, int> sSrc(kCapacity);

    auto c = [&] { cSrc.reserve(kReserveCount); };

    auto s = [&] { sSrc.reserve(kReserveCount); };

    BENCH("reserve()", c, s);
}

// Executes all reserve benchmark cases.
static void run_benchmarks() {
    bench_reserve();
}

REGISTER_BENCH_SUITE();
