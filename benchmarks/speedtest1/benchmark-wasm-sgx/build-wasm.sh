#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))
OUT_DIR=${ROOT_DIR}/src
OUT_FILE_WASM=${OUT_DIR}/benchmark-wasm.wasm
OUT_FILE_AOT=${OUT_DIR}/benchmark-wasm.aot

WASI_SDK_DIR=/opt/wasi-sdk
WASI_SYSROOT_DIR=${WASI_SDK_DIR}/share/wasi-sysroot
WASI_DEFINED_SYMBOLS_FILE=${WASI_SDK_DIR}/share/wasi-sysroot/share/wasm32-wasi/defined-symbols.txt
WAMR_AOT_COMPILER_DIR=/opt/wamr-sdk/wamr-compiler/build

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
${WASI_SDK_DIR}/bin/clang     \
        --target=wasm32-wasi -O3 \
        --sysroot=${WASI_SYSROOT_DIR} \
        -Wl,--allow-undefined-file=${WASI_DEFINED_SYMBOLS_FILE} \
        -Wl,--no-threads,--strip-all \
        -Wl,--export=main \
        -Wl,--export=speedtest1_initialize \
        -Wl,--export=speedtest1_launch100 \
        -Wl,--export=speedtest1_launch110 \
        -Wl,--export=speedtest1_launch120 \
        -Wl,--export=speedtest1_launch130 \
        -Wl,--export=speedtest1_launch140 \
        -Wl,--export=speedtest1_launch142 \
        -Wl,--export=speedtest1_launch145 \
        -Wl,--export=speedtest1_launch160 \
        -Wl,--export=speedtest1_launch161 \
        -Wl,--export=speedtest1_launch170 \
        -Wl,--export=speedtest1_launch180 \
        -Wl,--export=speedtest1_launch190 \
        -Wl,--export=speedtest1_launch210 \
        -Wl,--export=speedtest1_launch230 \
        -Wl,--export=speedtest1_launch240 \
        -Wl,--export=speedtest1_launch250 \
        -Wl,--export=speedtest1_launch260 \
        -Wl,--export=speedtest1_launch270 \
        -Wl,--export=speedtest1_launch280 \
        -Wl,--export=speedtest1_launch290 \
        -Wl,--export=speedtest1_launch300 \
        -Wl,--export=speedtest1_launch320 \
        -Wl,--export=speedtest1_launch400 \
        -Wl,--export=speedtest1_launch410 \
        -Wl,--export=speedtest1_launch500 \
        -Wl,--export=speedtest1_launch510 \
        -Wl,--export=speedtest1_launch520 \
        -Wl,--export=speedtest1_launch980 \
        -Wl,--export=speedtest1_launch990 \
        -DSQLITE_OS_OTHER \
        -DSQLITE_ENABLE_MEMSYS3 \
        -I${ROOT_DIR}/../benchmark/include \
        -I${ROOT_DIR}/../../sqlite/dist/sqlite \
        -o ${OUT_FILE_WASM} ${ROOT_DIR}/src-wasm/main.c ${ROOT_DIR}/../benchmark/src/*.c ${ROOT_DIR}/../../sqlite/dist/sqlite/*.c

if [ -f ${OUT_FILE_WASM} ]; then
        echo "build ${OUT_FILE_WASM} success"
else
        echo "build ${OUT_FILE_WASM} fail"
fi

# Check WAMR AoT compiler is present
if [ ! -d $WAMR_AOT_COMPILER_DIR ] 
then
    echo "Error: WAMR AoT compiler not located into $WAMR_AOT_COMPILER_DIR."
    echo "Stopping building benchmark-wasm-sgx."
    exit 1
fi

# Use WAMR AoT compiler to build out the .aot binary
$WAMR_AOT_COMPILER_DIR/wamrc -sgx -o ${OUT_FILE_AOT} ${OUT_FILE_WASM}

if [ -f ${OUT_FILE_AOT} ]; then
        echo "build ${OUT_FILE_AOT} success"
else
        echo "build ${OUT_FILE_AOT} fail"
fi