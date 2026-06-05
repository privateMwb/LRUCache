#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <cmath>

#include "LRUCache.h"

std::string center(const std::string& text, std::size_t width) {
	if(text.size() >= width) {
		return text;
	}

	auto left = (width - text.size()) / 2;
	auto right = width - text.size() - left;

	return std::string(left, ' ') + text + std::string(right, ' ');
}

template<typename T>
std::string format(const T& value, const std::string& suffix = "") {
	std::ostringstream oss;
	oss << value;

	return oss.str() + suffix;
}

std::string repeat(
    const std::string& line,
    std::size_t count)
{
	std::string lines;

	for(std::size_t i = 0; i < count; ++i) {
		lines += line;
	}

	return lines;
}

void border(
    const std::string& left,
    const std::string& middle,
    const std::string& right,
    const std::string& line,
    const std::vector<std::size_t>& lengths)
{
	std::cout << left;

	for(std::size_t i = 0; i < lengths.size(); ++i) {
		std::cout << repeat(line, lengths[i]) << (i < lengths.size() - 1 ? middle : "");
	}

	std::cout << right << "\n";
}

void table_header(
    const std::string& side,
    const std::vector<std::string>& titles,
    const std::vector<std::size_t>& lengths)
{
	if(titles.size() != lengths.size()) {
		return;
	}

	for(std::size_t i = 0; i < lengths.size(); ++i) {
		std::cout << side
		          << center(titles[i], lengths[i])
		          << (i == lengths.size() - 1 ? side + "\n" : "");
	}
}

void basic_insertion() {
	LRUCache<int, std::string> lru(5);

	std::vector<int> keys = {1, 5, 15, 78, 3};
	std::vector<std::string> values = {"Apple", "Banana", "Mango", "Grapes", "Orange"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	std::vector<std::string> items;

	for(std::size_t i = 0; i < keys.size(); ++i) {
		items.push_back(*lru.get(keys[i]));
	}

	border("┌", "─", "┐", "─", {40});
	table_header("│", {"LRU Cache(5) Basic Usage"}, {40});
	border("├", "┬", "┤", "─", {10, 14, 14});
	table_header("│", {"Keys", "Values", "Get"}, {10, 14, 14});
	border("├", "┼", "┤", "─", {10, 14, 14});

	for(std::size_t i = 0; i < keys.size(); ++i) {
		std::cout << "│"
		          << center(format(keys[i]), 10)
		          << "│"
		          << center(values[i], 14)
		          << "│"
		          << center(items[i], 14)
		          << "│\n";
	}

	border("└", "┴", "┘", "─", {10, 14, 14});

	std::cout << "\n";
}

void automatic_eviction() {
	LRUCache<int, std::string> lru(2);
	std::vector<int> keys = {10, 3, 61};
	std::vector<std::string> values = {"Dog", "Cat", "Mouse"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	std::vector<std::string> items;

	for(std::size_t i = 0; i < keys.size(); ++i) {
		std::string result = lru.contain(keys[i]) ? "✓" : "✗";

		items.push_back(result);
	}

	border("┌", "─", "┐", "─", {40});
	table_header("│", {"LRU Cache(2) Automatic Eviction"}, {40});
	border("├", "┬", "┤", "─", {10, 14, 14});
	table_header("│", {"Keys", "Values", "Contain"}, {10, 14, 14});
	border("├", "┼", "┤", "─", {10, 14, 14});

	for(std::size_t i = 0; i < keys.size(); ++i) {
		std::cout << "│"
		          << center(format(keys[i]), 10)
		          << "│"
		          << center(values[i], 14)
		          << "│"
		          << center(items[i], 16)
		          << "│\n";
	}

	border("└", "┴", "┘", "─", {10, 14, 14});

	std::cout << "\n";
}

void statistics() {
    LRUCache<int, std::string> lru(5);
	std::vector<int> keys = {200, 300, 400};
	std::vector<int> xKeys = {2, 300, 4, 200, 0, 400};
	std::vector<std::string> values = {"Earth", "Mars", "Jupiter"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}

	std::vector<std::string> hits;
	std::vector<std::string> misses;
	std::vector<std::string> rates;

	for(std::size_t i = 0; i < xKeys.size(); ++i) {
		lru.get(xKeys[i]);
        
		hits.push_back(format(lru.hitCount()));
		misses.push_back(format(lru.missCount()));
		rates.push_back(format(std::round(lru.hitRate()), "%"));
	}

	border("┌", "─", "┐", "─", {40});
	table_header("│", {"LRU Cache(5) Statistics"}, {40});
	border("├", "┬", "┤", "─", {10, 9, 9, 9});
	table_header("│", {"Get", "Hit", "Miss", "Rate"}, {10, 9, 9, 9});
	border("├", "┼", "┤", "─", {10, 9, 9, 9});

	for(std::size_t i = 0; i < xKeys.size(); ++i) {
		std::cout << "│"
		          << center(format(xKeys[i]), 10)
		          << "│"
		          << center(hits[i], 9)
		          << "│"
		          << center(misses[i], 9)
		   		  << "│"
		          << center(rates[i], 9)
		          << "│\n";
	}

	border("└", "┴", "┘", "─", {10, 9, 9, 9});

	std::cout << "\n";
}

void order() {
    LRUCache<int, std::string> lru(5);
	std::vector<int> keys = {56, 78, 99, 89, 21};
	std::vector<std::string> values = {"Blue", "Red", "Black", "Brown", "Yellow"};

	for(std::size_t i = 0; i < keys.size(); ++i) {
		lru.put(keys[i], values[i]);
	}
    
    lru.get(keys[0]);

	border("┌", "─", "┐", "─", {40});
	table_header("│", {"LRU Cache(5) Order"}, {40});
	border("├", "┬", "┤", "─", {19, 20});
	table_header("│", {"Most Recent Key", "Least Recent Key"}, {19, 20});
	border("├", "┼", "┤", "─", {19, 20});
    table_header("│", {format(*lru.mostRecentKey()), format(*lru.leastRecentKey())}, {19, 20});
	border("└", "┴", "┘", "─", {19, 20});
	std::cout << "\n";
}


int main() {
	basic_insertion();

	automatic_eviction();

	statistics();
	
	order();
}

