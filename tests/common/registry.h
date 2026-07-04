#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct TestSuite {
    int id;
    std::string name;
    void (*run)();
};

inline std::vector<TestSuite>& test_registry()
{
    static std::vector<TestSuite> registry;
    return registry;
}

inline int& next_test_suite_id()
{
    static int id = 1;
    return id;
}

struct TestRegistrar {
    TestRegistrar(const char* file, void (*run)())
    {
        test_registry().push_back({
            next_test_suite_id()++,
            std::filesystem::path(file).stem().string(),
            run
        });
    }
};