cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_STANDARD 11)
set(SQLITE_DIR ../../sqlite/dist/sqlite)

project(speedtest1_benchmark)

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
file(GLOB_RECURSE BENCHMARK_SOURCE_FILES src/*.c)

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

# Include all the source code
file(GLOB_RECURSE BENCHMARK_SOURCE_FILES src/*.c)

# Indicate to create an executable program for this project
add_executable(benchmark ${BENCHMARK_SOURCE_FILES})

# Include the header code files
include_directories(benchmark PUBLIC include)

# Link with the SQLite static library
target_link_libraries(benchmark sqlite)

# Additional compiler flags, such as the optimization one
target_compile_options(benchmark PUBLIC -O3)