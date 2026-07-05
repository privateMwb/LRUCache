# ============================================================
# CMakeLists.txt
# Build configuration for the CachePro library.
# ============================================================
#
# Sections:
#   1. C++ Configuration
#   2. CachePro Library
#   3. Tests
#   4. Benchmarks
#   5. Examples
#   6. Installation
#   7. Packaging
#
# ============================================================

cmake_minimum_required(VERSION 3.20)

project(
    CachePro
    VERSION 1.0.0
    LANGUAGES CXX
)


# ============================================================================
# C++ Configuration
# ============================================================================

# Configure the C++ language standard.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# CachePro (Header-only Library)
# ============================================================================

# Define the header-only CachePro library.
add_library(CachePro INTERFACE)

# Expose the public include directory to consumers.
target_include_directories(
    CachePro
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Require C++23 for all targets that consume CachePro.
target_compile_features(
    CachePro
    INTERFACE
    cxx_std_23
)

# ============================================================================
# Tests
# ============================================================================

# Collect all test source files.
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

# Build the test executable.
add_executable(
    tests
    ${TEST_SOURCES}
)

# Allow simple test includes.
target_include_directories(
    tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

# Link CachePro to the test executable.
target_link_libraries(
    tests
    PRIVATE
    CachePro
)


# ============================================================================
# Benchmarks
# ============================================================================

# Collect all benchmark source files.
file(GLOB_RECURSE BENCH_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/*.cpp"
)

# Build the benchmark executable.
add_executable(
    benchmarks
    ${BENCH_SOURCES}
)

# Allow simple benchmark includes.
target_include_directories(
    benchmarks
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks
)

# Link CachePro to the benchmark executable.
target_link_libraries(
    benchmarks
    PRIVATE
    CachePro
)


# ============================================================================
# Examples
# ============================================================================

# Collect all examples source files.
file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
)

# Build the example executable.
add_executable(
    examples
    ${EXAMPLE_SOURCES}
)

# Allow simple example includes.
target_include_directories(
    examples
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
)

# Link CachePro to the example executable.
target_link_libraries(
    examples
    PRIVATE
    CachePro
)


# ============================================================================
# Installation
# ============================================================================

# Install the CachePro target and export it for downstream projects.
install(
    TARGETS CachePro
    EXPORT CacheProTargets
)

# Install the exported CMake targets.
install(
    EXPORT CacheProTargets
    FILE CacheProTargets.cmake
    NAMESPACE CachePro::
    DESTINATION lib/cmake/CachePro
)

# Install the public header files.
install(
    DIRECTORY include/
    DESTINATION include
)

# ============================================================================
# Packaging
# ============================================================================

include(CMakePackageConfigHelpers)

# Generate the package configuration file.
configure_package_config_file(
    cmake/CacheProConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CacheProConfig.cmake
    INSTALL_DESTINATION lib/cmake/CachePro
)

# Generate the package version file.
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CacheProConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the package configuration files.
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/CacheProConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/CacheProConfigVersion.cmake
    DESTINATION lib/cmake/CachePro
)