# Set a variable that references the SQLite library
set(SQLITE_DIR ../../sqlite/dist/sqlite)


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
# Benchmark configuration
#

# Include all the source code
file(GLOB_RECURSE BENCHMARK_SOURCE_FILES src/*.c)

# Create a static library
add_library(benchmark ${BENCHMARK_SOURCE_FILES})

# Include the header code files
include_directories(benchmark PUBLIC include)

# Link with the SQLite static library
target_link_libraries(benchmark sqlite)

# Additional compiler flags, such as the optimization one
target_compile_options(benchmark PUBLIC -O3)