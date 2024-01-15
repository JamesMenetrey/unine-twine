#!/bin/bash

# exit when any command fails
set -e

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
WAMR_PATH="/opt/wamr"

patch_cmake() {
    CMAKE_PATH=$1
    git checkout -- $CMAKE_PATH
    sed -i -e 's/set (WAMR_BUILD_INTERP 1)/set (WAMR_BUILD_INTERP 0)/' $CMAKE_PATH
    sed -i -e 's/set (WAMR_BUILD_LIBC_BUILTIN 1)/set (WAMR_BUILD_LIBC_BUILTIN 0)/' $CMAKE_PATH
    sed -i -e 's/set (WAMR_BUILD_FAST_INTERP 1)/set (WAMR_BUILD_FAST_INTERP 0)/' $CMAKE_PATH
    sed -i -e 's/set (WAMR_BUILD_LIB_PTHREAD 1)/set (WAMR_BUILD_LIB_PTHREAD 0)/' $CMAKE_PATH
    sed -i -e 's/set (WAMR_BUILD_SGX_IPFS 0)/set (WAMR_BUILD_SGX_IPFS 1)/' $CMAKE_PATH
    sed -i -e 's/set (WAMR_BUILD_SIMD 0)/set (WAMR_BUILD_SIMD 1)/' $CMAKE_PATH
}

patch_make_sgx() {
    MAKEFILE_PATH=$1
    git checkout -- $MAKEFILE_PATH
    # Configuration patching
    sed -i -e 's/SGX_MODE ?= SIM/SGX_MODE = HW\nSGX_PRERELEASE = 1/' $MAKEFILE_PATH
    sed -i -e 's/SGX_DEBUG ?= 1/SGX_DEBUG = 0/' $MAKEFILE_PATH
    sed -i -e 's/SGX_COMMON_CFLAGS += -O2/SGX_COMMON_CFLAGS += -O3/' $MAKEFILE_PATH
    sed -i -e 's/SGX_MODE ?= SIM/SGX_MODE = HW\nSGX_PRERELEASE = 1/' $MAKEFILE_PATH

    # Optimized IPFS patching
    sed -i -e 's/protected_fs/protected_fs_benchmark/' $MAKEFILE_PATH
    sed -i -E "s/-L\\$\(SGX_LIBRARY_PATH\)/-L\\$\(SGX_LIBRARY_PATH\) -L\/linux-sgx\/build\/linux/" $MAKEFILE_PATH
}

patch_enclave_xml() {
    XML_PATH=$1
    HEAP_SIZE_MAX=$2
    STACK_MAX_SIZE=$3
    RESERVED_MEM_MAX_SIZE=$4
    git checkout -- $XML_PATH
    sed -i -e "s/<HeapMaxSize>.*<\/HeapMaxSize>/<HeapMaxSize>$HEAP_SIZE_MAX<\/HeapMaxSize>/" $XML_PATH
    sed -i -e "s/<StackMaxSize>.*<\/StackMaxSize>/<StackMaxSize>$STACK_MAX_SIZE<\/StackMaxSize>/" $XML_PATH
    sed -i -e "s/<ReservedMemMaxSize>.*<\/ReservedMemMaxSize>/<ReservedMemMaxSize>$RESERVED_MEM_MAX_SIZE<\/ReservedMemMaxSize>/" $XML_PATH
}

patch_enclave_edl() {
    EDL_PATH=$1
    git checkout -- $EDL_PATH
    sed -i -e "s/sgx_tprotected_fs.edl/\/linux-sgx\/common\/inc\/sgx_tprotected_fs.edl/" $EDL_PATH
}

echo "[+] Patch the compilation files"
cd $WAMR_PATH/product-mini/platforms/linux
patch_cmake CMakeLists.txt
cd $WAMR_PATH/product-mini/platforms/linux-sgx
patch_cmake CMakeLists.txt
cd $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample
patch_make_sgx Makefile
patch_enclave_xml Enclave/Enclave.config.xml $SGX_HEAP_SIZE_MAX $SGX_STACK_MAX_SIZE $SGX_RESERVED_MEM_MAX_SIZE
patch_enclave_edl Enclave/Enclave.edl

echo "[+] Building the Wasm runtime for Linux (iwasm)"
rm -rf $WAMR_PATH/product-mini/platforms/linux/build
mkdir -p $WAMR_PATH/product-mini/platforms/linux/build
cd $WAMR_PATH/product-mini/platforms/linux/build
cmake .. -DWAMR_BUILD_GLOBAL_HEAP_SIZE=$GLOBAL_HEAP_SIZE -DWAMR_BUILD_GLOBAL_HEAP_POOL=1
make all -j$(nproc)


echo "[+] Building the Wasm runtime for Intel SGX (vmlib)"
rm -rf $WAMR_PATH/product-mini/platforms/linux-sgx/build
mkdir -p $WAMR_PATH/product-mini/platforms/linux-sgx/build
cd $WAMR_PATH/product-mini/platforms/linux-sgx/build
cmake .. -DWAMR_BUILD_GLOBAL_HEAP_SIZE=$GLOBAL_HEAP_SIZE -DWAMR_BUILD_GLOBAL_HEAP_POOL=1
make -j$(nproc)

echo "[+] Building the Intel SGX enclave (iwasm)"
cd $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample
make clean
make

cd $WAMR_PATH/wamr-compiler
if [ ! -d "../core/deps/llvm" ]; then
    echo "[+] Building LLVM"
    ./build_llvm.sh
fi

echo "[+] Building the AoT compiler"
cd $WAMR_PATH/wamr-compiler
rm -rf build
mkdir -p build
cd build
cmake ..
make -j$(nproc)