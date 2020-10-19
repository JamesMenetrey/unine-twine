cmake_minimum_required(VERSION 3.17)

project(sqlite_benchmark_wasm)

set(CMAKE_C_STANDARD 11)

# Set variables that references the benchmark project
set(BENCHMARK_ROOT_DIR ../benchmark)
set(BENCHMARK_INCLUDE_DIR ${BENCHMARK_ROOT_DIR}/include)

################  runtime settings  ################
set (WAMR_BUILD_PLATFORM "linux")

# Reset default linker flags
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# WAMR features switch
set (WAMR_BUILD_TARGET "X86_64")
set (CMAKE_BUILD_TYPE Release)
set (WAMR_BUILD_INTERP 0)
set (WAMR_BUILD_AOT 1)
set (WAMR_BUILD_JIT 0)
set (WAMR_BUILD_LIBC_BUILTIN 0)
set (WAMR_BUILD_LIBC_WASI 1)
set (WAMR_BUILD_FAST_INTERP 0)

# linker flags
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -pie -fPIE")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat -Wformat-security")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mindirect-branch-register")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

# build out vmlib
set (WAMR_ROOT_DIR /opt/wamr-sdk)
include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)

add_library(vmlib ${WAMR_RUNTIME_LIB_SOURCE})

################  application related  ################
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
include (${SHARED_DIR}/utils/uncommon/shared_uncommon.cmake)

add_executable (benchmark-wasm src/main.c ${UNCOMMON_SHARED_SOURCE})
include_directories(benchmark-wasm PUBLIC ${BENCHMARK_INCLUDE_DIR})

target_link_libraries (benchmark-wasm vmlib -lm -ldl -lpthread -lrt)
