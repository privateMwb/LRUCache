#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>
#include <cstddef>

#include "LRUCache.h"

void basic_insertion() {
	LRUCache<int, std::string> lru(10);

	std::vector<int> keys = {1, 2, 3};
	std::vector<std::string> values = {"Apple", "Banana", "Mango"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	auto* item1 = lru.get(keys[0]);
	auto* item3 = lru.get(keys[2]);

	assert(item1 != nullptr);
	assert(*item1 == values[0]);

	assert(item3 != nullptr);
	assert(*item3 == values[2]);

	std::cout << "\n[PASS] Basic Insertion Test\n";
}

void lru_ordering() {
	LRUCache<int, std::string> lru(10);

	std::vector<int> keys = {1, 2, 3};
	std::vector<std::string> values = {"Scissor", "Paper", "Rock"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	lru.get(keys[1]);

	auto* most = lru.mostRecentKey();

	assert(most != nullptr);
	assert(*most == keys[1]);

	auto* least = lru.leastRecentKey();

	assert(least != nullptr);
	assert(*least == keys[0]);

	std::cout << "\n[PASS] LRU Ordering Test\n";
}

void eviction() {
	LRUCache<int, int> lru(10);

	std::vector<int> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	std::vector<int> values = {11, 22, 33, 44, 55, 66, 77, 88, 99, 100, 111};

	for(std::size_t i = 0; i < lru.capacity(); ++i) {
		lru.put(keys[i], values[i]);
	}

	lru.put(keys[10], values[10]);

	auto* key = lru.leastRecentKey();

	assert(key != nullptr);
	assert(*key == keys[1]);

	assert(lru.get(keys[0]) == nullptr);

	std::cout << "\n[PASS] Eviction Test\n";
}

void get_key() {
    LRUCache<int, std::string> lru(10);

	std::vector<int> keys = {-1, -2, -3, -4, -5};
	std::vector<std::string> values = {"Fire", "Air", "Earth", "Water", "Light"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}
	
	for(std::size_t i = 0; i < keys.size(); ++i) {
		auto* item = lru.get(keys[i]);
		
		assert(item != nullptr);
		assert(*item == values[i]);
	}
	
	std::cout << "\n[PASS] Get Key Test\n";
}

void update_key() {
	LRUCache<int, std::string> lru(10);

	std::vector<std::string> values = {"Car", "Ship", "Planes"};
	std::vector<int> keys = {1, 2, 3};

	lru.put(keys[2], values[0]);
	lru.put(keys[0], values[1]);
	lru.put(keys[2], values[2]);

	auto* item = lru.get(keys[2]);

	assert(item != nullptr);
	assert(*item == values[2]);

	auto* most = lru.mostRecentKey();

	assert(most != nullptr);
	assert(*most == keys[2]);

	std::cout << "\n[PASS] Update Key Test\n";
}

void erase_key() {
	LRUCache<int, std::string> lru(10);

	std::vector<int> keys = {21, 34, 87};
	std::vector<std::string> values = {"Cat", "Dog", "Mouse"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	lru.erase(keys[1]);

	auto* item = lru.get(keys[1]);

	assert(item == nullptr);

	assert(lru.size() == keys.size() - 1);

	std::cout << "\n[PASS] Erase Key Test\n";
}

void clear_keys() {
	LRUCache<int, std::string> lru(10);

	std::vector<int> keys = {312, 809, 69, 1, -1};
	std::vector<std::string> values = {"Smartphone", "Laptop", "Tablet", "Computer", "Smartwatch"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	assert(!lru.empty());

	lru.clear();

	assert(lru.empty());
	assert(lru.size() == 0);

	std::cout << "\n[PASS] Clear Keys Test\n";
}


void contain_key() {
	LRUCache<int, std::string> lru(10);

	std::vector<int> keys = {32, 89, 6, 971, 23};
	std::vector<std::string> values = {"Coke", "Sprite", "Pepsi", "Nestea", "Dew"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	for(std::size_t i = 0; i < keys.size(); ++i) {
		assert(lru.contain(keys[i]));
	}

	assert(!lru.contain(99999));

	std::cout << "\n[PASS] Contain Key Test\n";
}

void peek_key() {
	LRUCache<int, double> lru(10);

	std::vector<int> keys = {22, 189, 63, 91, -7, -22, 99, 0, 12, 999};
	std::vector<double> values = {1.01, 1.02, 1.03, 1.04, 1.05, 1.06, 1.07, 1.08, 1.09, 1.10};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	auto* peek = lru.peek(keys[5]);

	assert(peek != nullptr);
	assert(*peek == values[5]);

	auto* most = lru.mostRecentKey();

	assert(most != nullptr);
	assert(*most == keys[9]);

	std::cout << "\n[PASS] Peek Test\n";
}

void cache_statistics() {
	LRUCache<int, char> lru(10);

	std::vector<int> keys = {2, 567, -7, 191, -1000};
	std::vector<int> xKeys = {1, 872, 0, -78};
	std::vector<char> values = {'A', 'C', 'E', 'Z', 'Q'};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.get(keys[i]);
	}

	assert(lru.hitCount() == keys.size());
	assert(lru.missCount() == 0);

	for(std::size_t i = 0; i < xKeys.size(); ++i) {
		lru.get(xKeys[i]);
	}

	int total = lru.hitCount() + lru.missCount();
	double hitRate = (static_cast<double>(lru.hitCount()) / static_cast<double>(total)) * 100.0;

	assert(lru.hitCount() == keys.size());
	assert(lru.missCount() == xKeys.size());

	assert(std::abs(lru.hitRate() - hitRate) < 0.0001);

	std::cout << "\n[PASS] Cache Statistics Test\n";
}

void cache_reserve() {
	std::size_t cap = 10;
	LRUCache<int, std::string> lru(cap);

	lru.reserve(100);

	assert(lru.capacity() == cap);
	assert(lru.size() == 0);
	assert(lru.empty());

	std::vector<int> keys = {-2, 888, 100};
	std::vector<std::string> values = {"Black", "White", "Pink"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	for(std::size_t i = 0; i < keys.size(); ++i) {
		auto* item = lru.get(keys[i]);

		assert(item != nullptr);
		assert(*item == values[i]);
	}

	std::cout << "\n[PASS] Cache Reserve Test\n";
}

void capacity_state() {
	std::size_t cap = 1;
	LRUCache<int, std::string> lru(cap);

	std::vector<int> keys = {92, 869, 68, -971, -23};
	std::vector<std::string> values = {"Earth", "Mars", "Neptune", "Mercury", "Jupiter"};

	assert(lru.empty());

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	assert(lru.capacity() == cap);
	assert(lru.size() == cap);

	assert(*lru.mostRecentKey() == keys[keys.size() - 1]);
	assert(*lru.leastRecentKey() == keys[keys.size() - 1]);

	assert(!lru.empty());

	std::cout << "\n[PASS] Capacity & State Test\n";
}

void move_semantics() {
	{
		LRUCache<int, char> original(10);

		std::vector<int> keys = {-2, 3367, -87, 8, -100};
		std::vector<char> values = {'i', 'l', 'y', 's', 'm'};

		for(std::size_t i = 0; i < keys.size(); ++i) {
			original.put(keys[i], values[i]);
		}

		LRUCache<int, char> moved(std::move(original));

		for(std::size_t i = 0; i < keys.size(); ++i) {
			auto* item = moved.get(keys[i]);

			assert(item != nullptr);
			assert(*item == values[i]);
		}
	}

	{
		LRUCache<int, char> source(10);

		std::vector<int> keys = {-2, 3367, -87, 8, -100};
		std::vector<char> values = {'i', 'l', 'y', 's', 'm'};

		for(std::size_t i = 0; i < keys.size(); ++i) {
			source.put(keys[i], values[i]);
		}

		LRUCache<int, char> destination(5);
		destination = std::move(source);

		for(std::size_t i = 0; i < keys.size(); ++i) {
			auto* item = destination.get(keys[i]);

			assert(item != nullptr);
			assert(*item == values[i]);
		}
	}
	
	std::cout << "\n[PASS] Move Semantics Test\n";
}

int main() {
	basic_insertion();

	lru_ordering();

	eviction();
    
    get_key();
    
	update_key();

	erase_key();

	clear_keys();

	contain_key();

	peek_key();

	cache_statistics();

	cache_reserve();

	capacity_state();

	move_semantics();
	
	std::cout << "\nAll Test Completed\n";
	return 0;
}

