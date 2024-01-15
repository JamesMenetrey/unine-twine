#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../common.sh

set -e

build_optimized_ipfs
build_runtime $((60 * 1024 * 1024)) $((80 * 1024 * 1024)) $((4 * 1024 * 1024)) $((4 * 1024 * 1024))

announce "Building the Docker container for Speedtest1"
docker build $SCRIPT_DIR -t unine_speedtest1

announce "Compiling Speedtest1 native"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/sqlite:/sqlite \
    -v $ROOT_DIR/speedtest1:/speedtest1 \
    unine_speedtest1 \
    ./build-native.sh

announce "Compiling Speedtest1 native for SGX"
cd $cd $SCRIPT_DIR/benchmark-sgx
./build-benchmark.sh

announce "Compiling Speedtest1 Wasm"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/sqlite:/sqlite \
    -v $ROOT_DIR/speedtest1:/speedtest1 \
    unine_speedtest1 \
    ./build-wasm.sh