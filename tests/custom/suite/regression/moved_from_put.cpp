// LRUCache moved-from put() regression test suite.
//
// Coverage:
// - After a move (construction or assignment), the source cache is left
//   with capacity() == 0 and null pool/table pointers. put() does not
//   guard against this: findSlot()/insertSlot() index straight into a
//   null `table_` via `hash & tableMask_` (tableMask_ == 0), which is
//   undefined behavior rather than a clean failure.
// - This test documents the expected contract once fixed: put() on a
//   moved-from (zero-capacity) cache must fail safely — e.g. by throwing
//   std::invalid_argument — the same way the constructor already rejects
//   capacity 0, rather than dereferencing a null pointer.

#include <support/framework.h>

using namespace CachePro;

// Verifies put() on a moved-from cache (via move construction) fails safely.
static void put_after_move_construct_throws() {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");

    LRUCache<int, std::string> dest(std::move(source));

    // source is now moved-from: capacity() == 0, storage_/table_ == nullptr
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    CHK_THROWS(source.put(2, "b"), std::invalid_argument);
}

// Verifies put() on a moved-from cache (via move assignment) fails safely.
static void put_after_move_assign_throws() {
    LRUCache<int, std::string> source(4);
    source.put(1, "a");

    LRUCache<int, std::string> dest(2);
    dest = std::move(source);

    // source is now moved-from: capacity() == 0, storage_/table_ == nullptr
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    CHK_THROWS(source.put(2, "b"), std::invalid_argument);
}

// Executes all moved-from put() test cases.
static void run_tests() {
    RUN(put_after_move_construct_throws);
    RUN(put_after_move_assign_throws);
}

REGISTER_TEST_SUITE();
