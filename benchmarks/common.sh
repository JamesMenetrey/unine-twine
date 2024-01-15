#!/bin/bash

# User settings
ROOT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
WAMR_PATH="/opt/wamr/"

# Macros
announce () {
    echo "$(tput smso 2>/dev/null)>>> $1$(tput rmso 2>/dev/null)"
}

# Functions
# build_runtime
build_runtime() {
    GLOBAL_HEAP_SIZE=$1
    SGX_HEAP_SIZE_MAX=$2
    SGX_STACK_MAX_SIZE=$3
    SGX_RESERVED_MEM_MAX_SIZE=$4

    announce "Building AoT compiler"

    cd $ROOT_DIR

    # Build the environment for compiling the runtime
    docker build -t unine_wamr $ROOT_DIR/runtime

    # Compile the runtime
    docker run \
        --rm \
        -u $(id -u ${USER}):$(id -g ${USER}) \
        -v $ROOT_DIR/runtime:/runtime \
        -v $WAMR_PATH:$WAMR_PATH \
        -v $ROOT_DIR/linux-sgx:/linux-sgx \
        -e GLOBAL_HEAP_SIZE=$GLOBAL_HEAP_SIZE \
        -e SGX_HEAP_SIZE_MAX=$SGX_HEAP_SIZE_MAX \
        -e SGX_STACK_MAX_SIZE=$SGX_STACK_MAX_SIZE \
        -e SGX_RESERVED_MEM_MAX_SIZE=$SGX_RESERVED_MEM_MAX_SIZE \
        unine_wamr
}

# build_optimized_ipfs <sdk_flag>
build_optimized_ipfs() {
    SDK_FLAG=$1
    docker build -t unine_benchmark_linux-sgx $ROOT_DIR/linux-sgx

    docker run \
        --rm \
        -u $(id -u ${USER}):$(id -g ${USER}) \
        -v $ROOT_DIR/linux-sgx:/linux-sgx \
        -e BUILDING_SDK_FLAG=$SDK_FLAG \
        unine_benchmark_linux-sgx
}

# enable_large_pages
enable_large_pages() {
    sudo su -c "echo always > /sys/kernel/mm/transparent_hugepage/enabled"
}

# optin_large_pages
optin_large_pages() {
    sudo su -c "echo madvise > /sys/kernel/mm/transparent_hugepage/enabled"
}

# disable_large_pages
disable_large_pages() {
    sudo su -c "echo never > /sys/kernel/mm/transparent_hugepage/enabled"
}