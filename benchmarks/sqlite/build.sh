#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../common.sh

set -e

build_optimized_ipfs
build_runtime $((200 * 1024 * 1024)) $((205 * 1024 * 1024)) $((256 * 1024)) $((4 * 1024 * 1024))

announce "Building the Docker container for SQLite microbenchmarks"
docker build $SCRIPT_DIR -t unine_sqlite

announce "Compiling SQLite microbenchmarks Wasm"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/sqlite:/sqlite \
    unine_sqlite \
    ./build-wasm.sh $@

announce "Compiling SQLite microbenchmarks native for SGX"
cd $cd $SCRIPT_DIR/benchmark-sgx
./build-benchmark.sh

announce "Compiling SQLite microbenchmarks native"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/sqlite:/sqlite \
    unine_sqlite \
    ./build-native.sh $@