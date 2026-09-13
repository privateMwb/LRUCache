// LRUCache empty-source move regression test suite.
//
// Coverage:
// - Move construction/assignment branches on whether the source's list is
//   empty (`other.head_.next == &other.tail_`) versus populated, since an
//   empty list's sentinels point at each other rather than at real nodes
//   that can simply be re-pointed.
// - This suite pins down that moving from an already-empty cache takes
//   that branch correctly: the destination's sentinels end up properly
//   self-linked (not dangling at the source's now-moved-from address),
//   and the destination is fully usable afterward.

#include <support/framework.h>

using namespace CachePro;

// Verifies move-constructing from an empty source correctly initializes
// the destination's list, confirmed by subsequent put()s working correctly.
static void move_construct_from_empty_source_initializes_list() {
    LRUCache<int, std::string> source(4);

    LRUCache<int, std::string> dest(std::move(source));
    CHK(dest.mostRecentKey() == nullptr);
    CHK(dest.leastRecentKey() == nullptr);
    CHK(dest.empty());

    // If the sentinels were left dangling at the source's old addresses
    // rather than correctly self-linked, these would corrupt the list or
    // crash rather than behaving like a normal, freshly built cache.
    dest.put(1, "a");
    dest.put(2, "b");
    CHK(*dest.mostRecentKey() == 2);
    CHK(*dest.leastRecentKey() == 1);
    CHK(dest.size() == 2);
}

// Verifies move-assigning from an empty source correctly reinitializes the
// destination's list, even when the destination previously held entries.
static void move_assign_from_empty_source_initializes_list() {
    LRUCache<int, std::string> dest(4);
    dest.put(99, "stale");

    LRUCache<int, std::string> source(3);
    dest = std::move(source);

    CHK(dest.mostRecentKey() == nullptr);
    CHK(dest.leastRecentKey() == nullptr);
    CHK(dest.empty());
    CHK(!dest.contains(99));

    dest.put(1, "a");
    dest.put(2, "b");
    CHK(*dest.mostRecentKey() == 2);
    CHK(*dest.leastRecentKey() == 1);
}

// Verifies moving from an empty source into an also-empty destination
// still produces a correctly self-linked, usable cache.
static void move_between_two_empty_caches() {
    LRUCache<int, std::string> dest(2);
    LRUCache<int, std::string> source(5);

    dest = std::move(source);
    CHK(dest.capacity() == 5);
    CHK(dest.empty());

    dest.put(1, "a");
    CHK(*dest.mostRecentKey() == 1);
    CHK(*dest.leastRecentKey() == 1);
}

// Executes all empty-source move test cases.
static void run_tests() {
    RUN(move_construct_from_empty_source_initializes_list);
    RUN(move_assign_from_empty_source_initializes_list);
    RUN(move_between_two_empty_caches);
}

REGISTER_TEST_SUITE();
