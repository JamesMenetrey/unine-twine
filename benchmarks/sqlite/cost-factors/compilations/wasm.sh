#!/bin/bash

ROOT_DIR=/sqlite/benchmark-wasm
OUT_DIR=${ROOT_DIR}/build
OUT_FILE_WASM=${OUT_DIR}/benchmark-wasm.wasm

WASI_SDK_DIR=/opt/wasi-sdk
WASI_SYSROOT_DIR=${WASI_SDK_DIR}/share/wasi-sysroot
WASI_DEFINED_SYMBOLS_FILE=${WASI_SDK_DIR}/share/wasi-sysroot/share/wasm32-wasi/defined-symbols.txt

${WASI_SDK_DIR}/bin/clang     \
        --target=wasm32-wasi -O3 \
        --sysroot=${WASI_SYSROOT_DIR} \
        -Wl,--allow-undefined-file=${WASI_DEFINED_SYMBOLS_FILE} \
        -Wl,--no-threads,--strip-all \
        -Wl,--export=main \
        -Wl,--export=open_db \
        -Wl,--export=close_db \
        -Wl,--export=setup_database \
        -Wl,--export=init_rand \
        -Wl,--export=insert_data \
        -Wl,--export=query_data_sequential \
        -Wl,--export=query_data_random \
        -Wl,--export=print_memory_usage \
        -DSQLITE_OS_OTHER \
        -DSQLITE_ENABLE_MEMSYS3 \
        -I${ROOT_DIR}/../benchmark/include \
        -I${ROOT_DIR}/../dist/sqlite \
        -o ${OUT_FILE_WASM} ${ROOT_DIR}/src-wasm/main.c ${ROOT_DIR}/../benchmark/src/*.c ${ROOT_DIR}/../dist/sqlite/*.c