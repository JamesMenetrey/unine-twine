#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))
OUT_DIR=${SCRIPT_DIR}/build/wasm
OUT_FILE_WASM=${OUT_DIR}/benchmark-wasm.wasm
OUT_FILE_AOT=${OUT_DIR}/benchmark-wasm.aot

WASI_SDK_DIR=/opt/wasi-sdk
WASI_SYSSCRIPT_DIR=${WASI_SDK_DIR}/share/wasi-sysroot
WASI_DEFINED_SYMBOLS_FILE=${WASI_SDK_DIR}/share/wasi-sysroot/share/wasm32-wasi/defined-symbols.txt
WAMR_AOT_COMPILER_DIR=/opt/wamr/wamr-compiler/build

INSTRUMENTATION_SRC=timing_callbacks

# Check WASI SDK is present
if [ ! -d $WASI_SDK_DIR ] 
then
    echo "Error: WASI SDK not located into $WASI_SDK_DIR."
    echo "Stopping building benchmark-wasm."
    exit 1
fi

mkdir -p ${OUT_DIR}
rm -f ${OUT_FILE_WASM} ${OUT_FILE_AOT}

# Use WASI SDK to build out the .wasm binary
${WASI_SDK_DIR}/bin/clang \
        --target=wasm32-wasi \
        -O3 \
        --sysroot=${WASI_SYSSCRIPT_DIR} \
        -Wl,--export=malloc \
        -Wl,--export=free \
        -Wl,--allow-undefined-file=${WASI_DEFINED_SYMBOLS_FILE} \
        -Wl,--strip-all \
        -DSQLITE_OS_OTHER \
        -DSQLITE_ENABLE_MEMSYS3 \
        -I${SCRIPT_DIR}/benchmark/include \
        -I${SCRIPT_DIR}/../sqlite/dist/sqlite \
        -o ${OUT_FILE_WASM} \
        ${SCRIPT_DIR}/benchmark/src/*.c \
        ${SCRIPT_DIR}/benchmark/src/instrumentation-callbacks/${INSTRUMENTATION_SRC}.c \
        ${SCRIPT_DIR}/../sqlite/dist/sqlite/*.c
        

if [ -f ${OUT_FILE_WASM} ]; then
        echo "build ${OUT_FILE_WASM} success"
else
        echo "build ${OUT_FILE_WASM} fail"
fi

# Check WAMR AoT compiler is present
if [ ! -d $WAMR_AOT_COMPILER_DIR ] 
then
    echo "Error: WAMR AoT compiler not located into $WAMR_AOT_COMPILER_DIR."
    echo "Stopping building benchmark-wasm."
    exit 1
fi

# Use WAMR AoT compiler to build out the .aot binary
$WAMR_AOT_COMPILER_DIR/wamrc --bounds-checks=0 -sgx -o ${OUT_FILE_AOT} ${OUT_FILE_WASM}

if [ -f ${OUT_FILE_AOT} ]; then
        echo "build ${OUT_FILE_AOT} success"
else
        echo "build ${OUT_FILE_AOT} fail"
fi