#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))
WAMR_AOT_COMPILER_DIR=/opt/wamr-sdk/wamr-compiler

echo "[+] Compiler the WAMR (UniNE) AoT compiler"
# Check WAMR AoT compiler exists
if [ ! -d $WAMR_AOT_COMPILER_DIR ] 
then
    echo "Error: WAMR AoT compiler not located into $WAMR_AOT_COMPILER_DIR."
    echo "Stopping building benchmark."
    exit 1
fi
cd /opt/wamr-sdk/wamr-compiler
./build_llvm.sh
rm -rf build
mkdir -p build
cd build
cmake ..
make

echo "[+] Building benchmark-native"
rm -rf $ROOT_DIR/benchmark/build
rm -rf $ROOT_DIR/benchmark-native/build
mkdir -p $ROOT_DIR/benchmark-native/build
cd $ROOT_DIR/benchmark-native/build
cmake ..
make

echo "[+] Building benchmark-wasm"
rm -rf $ROOT_DIR/benchmark-wasm/build
mkdir -p $ROOT_DIR/benchmark-wasm/build
cd $ROOT_DIR/benchmark-wasm/build
../build-wasm.sh
../build-benchmark.sh

echo "[+] Building benchmark-wasm-sgx"
rm -rf $ROOT_DIR/benchmark-wasm-sgx/build
mkdir -p $ROOT_DIR/benchmark-wasm-sgx/build
cd $ROOT_DIR/benchmark-wasm-sgx/build
../build-wasm.sh
../build-benchmark.sh