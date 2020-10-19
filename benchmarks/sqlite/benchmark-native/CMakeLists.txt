cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(sqlite_benchmark_native)

# Set variables that references the benchmark project
set(BENCHMARK_ROOT_DIR ../benchmark)
set(BENCHMARK_BUILD_DIR ../${BENCHMARK_ROOT_DIR}/build)
set(BENCHMARK_INCLUDE_DIR ${BENCHMARK_ROOT_DIR}/include)

# Include the CMake build of the benchmark project
add_subdirectory(${BENCHMARK_ROOT_DIR} ${BENCHMARK_BUILD_DIR})

# Include all the source code files for this project
file(GLOB_RECURSE BENCHMARK_NATIVE_SOURCE_FILES src/*.c)

# Indicate to create an executable program for this project
add_executable(benchmark-native ${BENCHMARK_NATIVE_SOURCE_FILES})

# Include the header code files of the benchmark project to be used in this project
include_directories(benchmark-native PUBLIC ${BENCHMARK_INCLUDE_DIR})

# Link the benchmark project with this project
target_link_libraries(benchmark-native benchmark)

# Additional compiler flags, such as the optimization one
target_compile_options(benchmark-native PUBLIC -O3)