cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(sqlite_benchmark_native_launch_time)

set(INSTRUMENTATION_SRC ../instrumentation-callbacks/src/launch_time_callbacks.c)

add_subdirectory(../../../benchmark-native benchmark-native)