#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct BenchSuite {
    int id;
    std::string name;
    void (*run)();
};

inline std::vector<BenchSuite>& bench_registry()
{
    static std::vector<BenchSuite> registry;
    return registry;
}

inline int& next_bench_suite_id()
{
    static int id = 1;
    return id;
}

struct BenchRegistrar {
    BenchRegistrar(const char* file, void (*run)())
    {
        bench_registry().push_back({
            next_bench_suite_id()++,
            std::filesystem::path(file).stem().string(),
            run
        });
    }
};