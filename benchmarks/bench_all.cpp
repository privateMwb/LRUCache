#include <iostream>
#include <string>

void run_put_benchmarks();
void run_get_benchmarks();
void run_eviction_benchmarks();
void run_erase_benchmarks();
void run_emplace_benchmarks();
void run_mixed_workload_benchmarks();
void run_resize_benchmarks();
void run_scaling_benchmarks();

struct BenchSuite {
  std::string_view name;
  void (*run)();
};

static constexpr BenchSuite suites[] = {
  {"put", run_put_benchmarks },
  {"get", run_get_benchmarks },
  {"eviction", run_eviction_benchmarks},
  {"erase", run_erase_benchmarks},
  {"emplace", run_emplace_benchmarks},
  {"mixed_workload", run_mixed_workload_benchmarks},
  {"resize", run_resize_benchmarks},
  {"scaling", run_scaling_benchmarks}
};

int main(int argc, char* argv[]) {
  if (argc == 1) {
    for (const auto& suite : suites)
       suite.run();
    return 0;
  }
  
  std::string_view requested = argv[1];
  
  if (requested == "list") {
    std::cout << "\nAvailable bench suites:\n";
    for (const auto& suite :  suites)
      std::cout <<  "   " 
                << suite.name 
                << "\n";
    std::cout << "\n";
    return 0;
  }
  
  for (const auto& suite : suites) {
    if (suite.name == requested) {
      suite.run();
      return 0;
    }
  }
  
  std::cerr << "\nUnknown bench suite: "
            << requested
            << "\n\n";
  return 1;
}