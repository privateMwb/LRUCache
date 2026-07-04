#include "helper.h"

// Executes a test function and prints a success message.
#define RUN(name) do {                          \
    try {                                       \
        int f = fail;                           \
        name();                                 \
        if(f == fail){                          \
            std::cout << GREEN                  \
                      << "[PASS] "              \
                      << RESET                  \
                      << prettify(#name)        \
                      << "\n";                  \
        }                                       \
    } catch (const std::exception& e) {         \
        ++fail;                                 \
        std::cout << RED                        \
                  << "[FAIL] "                  \
                  << RESET                      \
                  << prettify(#name)            \
                  << " threw: "                 \
                  << e.what()                   \
                  << "\n";                      \
    } catch (...) {                             \
        ++fail;                                 \
        std::cout << RED                        \
                  << "[FAIL] "                  \
                  << RESET                      \
                  << prettify(#name)            \
                  << " threw unknown\n";        \
    }                                           \
} while (0)

// Evaluates a test condition and updates the test statistics.
#define CHK(expr) do {                  \
    if (!(expr)) {                      \
        ++fail;                         \
        std::cout << RED                \
                  << "[FAIL] "          \
                  << RESET              \
                  << "("                \
                  << __FILE__           \
                  << ":"                \
                  << __LINE__           \
                  << ")\n";             \
    } else ++pass;                      \
    ++total;                            \
} while (0)


#define REGISTER_TEST_SUITE()        \
    namespace {                      \
        static TestRegistrar registrar( \
            __FILE__,                \
            run_tests                \
        );                           \
    }