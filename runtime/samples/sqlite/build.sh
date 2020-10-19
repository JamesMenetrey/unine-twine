#!/bin/bash

CURR_DIR=$PWD
WAMR_DIR=${PWD}/../..
OUT_DIR=${PWD}/out


rm -rf ${OUT_DIR}
mkdir ${OUT_DIR}
mkdir ${OUT_DIR}/wasm-apps

WASI_SDK_DIR=/opt/wasi-sdk
WASI_SYSROOT_DIR=${WASI_SDK_DIR}/share/wasi-sysroot
WASI_DEFINED_SYMBOLS_FILE=${WASI_SDK_DIR}/share/wasi-sysroot/share/wasm32-wasi/defined-symbols.txt


echo "##################### build hosting app project"
cd ${CURR_DIR}
mkdir -p cmake_build
cd cmake_build
cmake ..
make
if [ $? != 0 ];then
    echo "BUILD_FAIL hosting app exit as $?\n"
    exit 2
fi

cp -a app ${OUT_DIR}

echo -e "\n"

echo "##################### build wasm apps"
cd ${CURR_DIR}

OUT_FILE=sqlite-test.wasm

# use WAMR SDK to build out the .wasm binary
/opt/wasi-sdk/bin/clang     \
        --target=wasm32-wasi -O0 \
        --sysroot=${WASI_SYSROOT_DIR} \
        -Wl,--allow-undefined-file=${WASI_DEFINED_SYMBOLS_FILE} \
        -Wl,--no-threads,--strip-all \
        -Wl,--export=main \
        -Wl,--allow-undefined \
        -DSQLITE_OS_OTHER \
        -o ${OUT_DIR}/wasm-apps/${OUT_FILE} dist/sqlite/*.c wasm-apps/sqlite-test.c

if [ -f ${OUT_DIR}/wasm-apps/${OUT_FILE} ]; then
        echo "build ${OUT_FILE} success"
else
        echo "build ${OUT_FILE} fail"
fi

echo "#################### build wasm apps done"
