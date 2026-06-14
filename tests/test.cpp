// LRUCache Test Suite
// Verifies correctness of the LRU cache container through unit tests:
//
// - lifecycle (default and move construction)
// - cache operations (put, erase, clear)
// - cache lookup (get, peek, contains)
// - capacity management (resize)
// - element access (keys, mostRecentKey, leastRecentKey)
// - cache statistics (hitCount, missCount, hitRate, resetStats)
// - capacity and size tracking
// - state (empty)
//
// These tests validate the correctness and expected behavior of LRUCache.

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "LRUCache.h"

void print(const std::string title) {
	std::cout << "[PASS] " << title << "\n";
}

// Lifecycle Test
// Verifies default and move construction.
void lifecycle() {
	std::cout << "Lifecycle Test\n";

	LRUCache<int, int> lru1(10);

	lru1.put(1, 10);
	lru1.put(2, 20);

	assert(lru1.size() == 2);
	assert(lru1.capacity() == 10);
	assert(lru1.contains(2));

	print("Default Construction");

	LRUCache<int, int> lru3(std::move(lru1));

	lru3.put(3, 30);

	assert(lru1.size() == 0);
	assert(lru1.capacity() == 0);
	assert(lru1.empty());
	assert(lru3.size() == 3);
	assert(lru3.capacity() == 10);

	print("Move Construction");

	std::cout << "\n";
}

// Cache Operations Test
// Verifies put, erase, and clear behavior.
void cacheOperations() {
	std::cout << "Cache Operation Test\n";

	LRUCache<int, double> lru(10);

	lru.put(10, 10.34);
	lru.put(98, 98.21);
	lru.put(45, 0.0001);

	assert(lru.size() == 3);
	assert(lru.contains(10));
	assert(lru.contains(45));

	lru.put(10, 99.99);

	assert(lru.size() == 3);
	assert(*lru.peek(10) == 99.99);

	print("Put");

	assert(lru.erase(98));
	assert(!lru.erase(100));
	assert(lru.size() == 2);

	print("Erase");

	lru.clear();

	assert(lru.capacity() == 10);
	assert(lru.empty());

	print("Clear");

	std::cout << "\n";
}

// Cache Lookup Test
// Verifies get, peek, and contains behavior.
void cacheLookup() {
	std::cout << "Cache Lookup Test\n";

	LRUCache<int, std::string> lru(10);

	lru.put(21, "apple");
	lru.put(98, "banana");
	lru.put(-45, "mango");

	auto get1 = lru.get(21);
	auto get2 = lru.get(100);

	assert(get1 != nullptr);
	assert(*get1 == "apple");
	assert(get2 == nullptr);

	print("Get");

	auto peek1 = lru.peek(98);
	auto peek2 = lru.peek(-45);
	auto peek3 = lru.peek(100);

	assert(peek1 != nullptr);
	assert(*peek1 == "banana");
	assert(peek2 != nullptr);
	assert(*peek2 == "mango");
	assert(peek3 == nullptr);

	assert(*lru.mostRecentKey() == 21);

	print("Peek");

	assert(lru.contains(98));
	assert(!lru.contains(100));

	print("Contains");

	std::cout << "\n";
}

// Capacity Management Test
// Verifies resize behavior and its error cases.
void capacityManagement() {
	std::cout << "Capacity Management Test\n";

	LRUCache<char, int> lru(10);

	lru.put('x', 21);
	lru.put('y', 33);
	lru.put('z', 55);

	assert(lru.capacity() == 10);

	lru.resize(20);

	assert(lru.capacity() == 20);

	try {
		lru.resize(0);
		assert(false);
	} catch (std::invalid_argument&) {}

	try {
		lru.resize(2);
		assert(false);
	} catch (std::length_error&) {}

	print("Resize");

	std::cout << "\n";
}

// Element Access Test
// Verifies keys(), mostRecentKey(), and leastRecentKey() behavior,
// including that peek() does not affect LRU ordering.
void elementAccess() {
	std::cout << "Element Access\n";

	LRUCache<char, double> lru(10);

	lru.put('a', 1.01);
	lru.put('b', 2.02);
	lru.put('c', 3.03);
	lru.put('d', 4.04);
	lru.put('e', 5.05);

	std::vector<char> keys = lru.keys();

	assert(keys[1] == 'd');
	assert(keys[3] == 'b');
	assert(keys.size() == lru.size());

	print("Keys");

	assert(*lru.mostRecentKey() == 'e');

	(void)lru.get('a');
	(void)lru.peek('c');

	assert(*lru.mostRecentKey() == 'a');

	(void)lru.get('c');

	assert(*lru.mostRecentKey() == 'c');

	print("Most Recent Key");

	assert(*lru.leastRecentKey() == 'b');

	(void)lru.peek('b');

	assert(*lru.leastRecentKey() == 'b');

	(void)lru.get('b');
	(void)lru.get('c');
	(void)lru.get('d');

	assert(*lru.leastRecentKey() == 'e');

	print("Least Recent Key");

	std::cout << "\n";
}

// Cache Statistics Test
// Verifies hitCount, missCount, hitRate, and resetStats behavior.
void cacheStatistics() {
	std::cout << "Cache Statistics\n";

	LRUCache<char, std::string> lru(10);

	lru.put('x', "xox");
	lru.put('y', "yoy");
	lru.put('z', "zoz");

	assert(lru.hitCount() == 0);

	(void)lru.get('x');
	(void)lru.get('z');

	assert(lru.hitCount() == 2);

	print("Hit Count");

	assert(lru.missCount() == 0);

	(void)lru.get('y');
	(void)lru.get('a');
	(void)lru.get('b');

	assert(lru.missCount() == 2);
	assert(lru.hitCount() == 3);

	print("Miss Count");

	double total = static_cast<double>(lru.hitCount() + lru.missCount());
	double rate  = (static_cast<double>(lru.hitCount()) / total) * 100.0;

	assert(lru.hitRate() == rate);

	print("Hit Rate");

	lru.resetStats();

	assert(lru.hitCount() == 0);
	assert(lru.missCount() == 0);
	assert(lru.hitRate() == 0.0);

	print("Reset Stats");

	std::cout << "\n";
}

// Capacity Test
// Verifies capacity tracking, eviction at capacity limit, and size changes.
void capacity() {
	std::cout << "Capacity\n";

	LRUCache<int, int> lru(1);

	assert(lru.capacity() == 1);

	lru.put(1, 500);
	lru.put(2, 1000);

	assert(lru.capacity() == 1);
	assert(!lru.contains(1));

	lru.resize(5);

	assert(lru.capacity() == 5);

	print("Capacity");

	assert(lru.size() == 1);

	lru.put(3, 1500);

	assert(lru.size() == 2);

	lru.clear();

	assert(lru.size() == 0);

	print("Size");

	std::cout << "\n";
}

// State Test
// Verifies empty() reflects the correct state.
void state() {
	std::cout << "State\n";

	LRUCache<int, int> lru(10);

	assert(lru.empty());

	lru.put(21, 780);

	assert(!lru.empty());

	print("Empty");
}

int main() {
	lifecycle();
	cacheOperations();
	cacheLookup();
	capacityManagement();
	elementAccess();
	cacheStatistics();
	capacity();
	state();

	return 0;
}

