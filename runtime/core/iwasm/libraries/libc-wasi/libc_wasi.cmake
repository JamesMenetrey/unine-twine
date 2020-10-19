# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set (LIBC_WASI_DIR ${CMAKE_CURRENT_LIST_DIR})

add_definitions (-DWASM_ENABLE_LIBC_WASI=1)

include_directories(${LIBC_WASI_DIR}/sandboxed-system-primitives/include)

# Common resources
include_directories(${LIBC_WASI_DIR}/sandboxed-system-primitives/src)
file (GLOB_RECURSE source_common ${LIBC_WASI_DIR}/libc_wasi_wrapper.c ${LIBC_WASI_DIR}/sandboxed-system-primitives/src/common/*.c)

if (DEFINED WAMR_BUILD_SGX)
    # SGX
    add_definitions (-DWASM_ENABLE_SGX=1)
    include_directories(${LIBC_WASI_DIR}/sandboxed-system-primitives/src/sgx)
    file (GLOB_RECURSE source_specific ${LIBC_WASI_DIR}/libc_wasi_benchmark.c ${LIBC_WASI_DIR}/sandboxed-system-primitives/src/sgx/*.c)

    if (BENCHMARK_PROFILING EQUAL 1)
        add_definitions (-DBENCHMARK_PROFILING=1)
        message("     Benchmark profiling enabled")
    endif()
else()
    # POSIX
    include_directories(${LIBC_WASI_DIR}/sandboxed-system-primitives/src/posix)
    file (GLOB_RECURSE source_specific ${LIBC_WASI_DIR}/sandboxed-system-primitives/src/posix/*.c)
endif()

set (LIBC_WASI_SOURCE ${source_common} ${source_specific})