#!/bin/bash

ROOT_DIR=/sqlite/benchmark-wasm
OUT_DIR=${ROOT_DIR}/build
OUT_FILE_WASM=${OUT_DIR}/benchmark-wasm.wasm
OUT_FILE_AOT=${OUT_DIR}/benchmark-wasm.aot
WAMR_AOT_COMPILER_DIR=/opt/wamr-sdk/wamr-compiler/build

$WAMR_AOT_COMPILER_DIR/wamrc -sgx -o ${OUT_FILE_AOT} ${OUT_FILE_WASM}

if [ -f ${OUT_FILE_AOT} ]; then
        echo "build ${OUT_FILE_AOT} success"
else
        echo "build ${OUT_FILE_AOT} fail"
fi