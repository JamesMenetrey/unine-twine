# Copyright (C) 2019 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

WAMR_DIR=${PWD}/../../..

WASI_SDK_DIR=/opt/wasi-sdk
WASI_SYSROOT_DIR=${WASI_SDK_DIR}/share/wasi-sysroot
WASI_DEFINED_SYMBOLS_FILE=${WASI_SDK_DIR}/share/wasi-sysroot/share/wasm32-wasi/defined-symbols.txt

/opt/wasi-sdk/bin/clang     \
        --target=wasm32-wasi -O0 \
        --sysroot=${WASI_SYSROOT_DIR}    \
        -Wl,--allow-undefined-file=${WASI_DEFINED_SYMBOLS_FILE} \
        -Wl,--export=main, \
        -Wl,--no-threads,--strip-all \
        -o test.wasm *.c
#./jeffdump -o test_wasm.h -n wasm_test_file test.wasm
