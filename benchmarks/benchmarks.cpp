#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef>

#include "LRUCache.h"

auto clock_now() {
	return std::chrono::steady_clock::now();
}

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

void put_get() {
	std::vector<std::size_t> caps = {100'000, 500'000, 1'000'000, 5'000'000, 10'000'000};
	std::vector<long long> putDurations;
	std::vector<long long> getDurations;

	for(std::size_t run = 0; run < caps.size(); ++run) {
		LRUCache<std::size_t, std::size_t> lru(caps[run]);
		lru.reserve(caps[run]);

		auto putStart = clock_now();

		for(std::size_t i = 0; i < caps[run]; ++i) {
			lru.put(i, i);
		}

		auto putEnd = clock_now();

		auto putDuration = std::chrono::duration_cast<std::chrono::milliseconds>(putEnd - putStart).count();

		putDurations.push_back(putDuration);

		auto getStart = clock_now();

		for(std::size_t i = 0; i < caps[run]; ++i) {
			lru.get(i);
		}

		auto getEnd = clock_now();

		auto getDuration = std::chrono::duration_cast<std::chrono::milliseconds>(getEnd - getStart).count();

		getDurations.push_back(getDuration);

	}

	border("┌", "─", "┐", "─", {48});
	table_header("│", {"LRU Cache Put/Get Benchmark"}, {48});
	border("├", "┬", "┤", "─", {18, 14, 14});
	table_header("│", {"Capacity", "Put", "Get"}, {18, 14, 14});
	border("├", "┼", "┤", "─", {18, 14, 14});

	for(std::size_t i = 0; i < caps.size(); ++i) {
		std::cout << std::left
		          << "│"
		          << center(format(caps[i]), 18)
		          << "│"
		          << center(format(putDurations[i], " ms"), 14)
		          << "│"
		          << center(format(getDurations[i], " ms"), 14)
		          << "│\n";
	}

	border("└", "┴", "┘", "─", {18, 14, 14});

    std::cout << "\n";
}

void mixed_workload() {
    constexpr std::size_t operations = 1'000'000;
    std::vector<std::size_t> caps = {100'000, 200'000, 300'000, 400'000, 500'000};
    std::vector<std::size_t> divs = {2, 5, 10, 4, 20};
    
    std::vector<long long> durations;
    
    for(std::size_t run = 0; run <caps.size(); ++run) {
        LRUCache<int, int> lru(caps[run]);
        
        for(std::size_t i = 0; i < caps[run]; ++i) {
            lru.put(i, i);
        }
        
        auto start = clock_now();
        
        for(std::size_t i = 0; i < operations; ++i) {
            if(i % divs[run] == 0) {
                lru.put(caps[run] + i, i);
            } else {
                lru.get(i % caps[run]);
            }
        }
        
        auto end = clock_now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        durations.push_back(duration);
    }
    
    border("┌", "─", "┐", "─", {54});
	table_header("│", {"LRU Cache Mixed Workload Benchmark"}, {54});
	border("├", "┬", "┤", "─", {18, 20, 14});
	table_header("│", {"Capacity", "Operations", "Duration"}, {18, 20, 14});
	border("├", "┼", "┤", "─", {18, 20, 14});

	for(std::size_t i = 0; i < caps.size(); ++i) {
		double putRate = (1.00 / static_cast<double>(divs[i])) * 100;
		double getRate = 100.00 - putRate;
		
		std::string operation = format(putRate, "% put") + " " + format(getRate, "% get");
		
		std::cout << std::left
		          << "│"
		          << center(format(caps[i]), 18)
		          << "│"
		          << center(operation, 20)
		          << "│"
		          << center(format(durations[i], " ms"), 14)
		          << "│\n";
	}

	border("└", "┴", "┘", "─", {18, 20, 14});
    
}

int main() {
	put_get();
	
	mixed_workload();
	return 0;
}

