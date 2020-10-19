#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

WAMR_ROOT_DIR=/opt/wamr-sdk
SGX_SDK_ROOT_DIR=/opt/intel/sgxsdk

# Check WASI SDK is present
if [ ! -d $WAMR_ROOT_DIR ] 
then
    echo "Error: WAMR SDK not located into $WAMR_ROOT_DIR."
    echo "Stopping building benchmark-wasm-sgx."
    exit 1
fi

# Check SGX SDK is present
if [ ! -d $SGX_SDK_ROOT_DIR ]
then
    echo "Error: SGX SDK not located into $SGX_SDK_ROOT_DIR."
    echo "Stopping building benchmark-wasm-sgx."
    exit 1
fiho "Profiling of the benchmark enabled"
fi

rm -rf ${ROOT_DIR}/build
mkdir -p ${ROOT_DIR}/build
cd ${ROOT_DIR}/build
cmake ..
make

cd ${ROOT_DIR}/src
make clean
make