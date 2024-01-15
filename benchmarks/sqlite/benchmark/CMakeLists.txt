cmake_minimum_required(VERSION 3.10)

project(sqlite_benchmark)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# Set a variable that references the SQLite library
set(SQLITE_DIR ../dist/sqlite)


#
# SQLite configuration
#

# Include all the source code in the SQLite project
file(GLOB SQLITE_SOURCE_FILES ${SQLITE_DIR}/*.c)

# Create a static library
add_library(sqlite STATIC ${SQLITE_SOURCE_FILES})

# Include the header code files
include_directories(sqlite PUBLIC ${SQLITE_DIR})

# Define a preprocessor directive to indicate SQLite to rely on another VFS
# and to use an alternate memory allocator
target_compile_definitions(sqlite PUBLIC SQLITE_OS_OTHER SQLITE_ENABLE_MEMSYS3)

# Additional compiler flags, such as the optimization one
target_compile_options(sqlite PUBLIC -O3)


#
# Benchmark library configuration
#

# Include all the source code
file(GLOB BENCHMARK_SOURCE_FILES src/benchmark.c src/random.c)

# Create a static library
add_library(benchmarklib ${BENCHMARK_SOURCE_FILES})

# Include the header code files
include_directories(benchmarklib PUBLIC include)

# Link with the SQLite static library
target_link_libraries(benchmarklib sqlite)

# Additional compiler flags, such as the optimization one
target_compile_options(benchmarklib PUBLIC -O3)


#
# Benchmark executable configuration
#

# Import the instrumentation callbacks dependencies
set(INSTRUMENTATION_DIR src/instrumentation-callbacks)
set(INSTRUMENTATION_INCLUDE ${INSTRUMENTATION_DIR}/include)
if (NOT DEFINED INSTRUMENTATION_SRC)
    set(INSTRUMENTATION_SRC ${INSTRUMENTATION_DIR}/timing_callbacks.c)
endif()
message(${INSTRUMENTATION_SRC})
# Include all the source code
file(GLOB BENCHMARK_SOURCE_FILES src/*.c ${INSTRUMENTATION_SRC})

# Create a executable
add_executable(benchmark ${BENCHMARK_SOURCE_FILES})

# Include the header code files
include_directories(benchmark PUBLIC include)

# Link with the SQLite static library
target_link_libraries(benchmark sqlite)

# Additional compiler flags, such as the optimization one
target_compile_options(benchmark PUBLIC -O3)