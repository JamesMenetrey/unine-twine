cmake_minimum_required(VERSION 3.17)

project(sqlite_benchmark_wasm_memory)

set(INSTRUMENTATION_SRC ../instrumentation-callbacks/src/memory_callbacks.c)

add_subdirectory(../../../benchmark-wasm benchmark-wasm)