#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../common.sh

set -e

build_optimized_ipfs
build_runtime $((60 * 1024 * 1024)) $((80 * 1024 * 1024)) $((4 * 1024 * 1024)) $((4 * 1024 * 1024))

announce "Building the Docker container for WolfSSL TLS"
docker build $SCRIPT_DIR -t unine_wolfssl_tls

announce "Compiling WolfSSL TLS client"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/tls:/tls \
    -v $ROOT_DIR/wolfssl:/wolfssl \
    unine_wolfssl_tls \
    ./build-client.sh

announce "Compiling WolfSSL TLS native"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/tls:/tls \
    -v $ROOT_DIR/wolfssl:/wolfssl \
    unine_wolfssl_tls \
    ./build-native.sh

announce "Compiling WolfSSL TLS Wasm"
cd $SCRIPT_DIR
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $WAMR_PATH:$WAMR_PATH \
    -v $ROOT_DIR/tls:/tls \
    -v $ROOT_DIR/wolfssl:/wolfssl \
    unine_wolfssl_tls \
    ./build-wasm.sh

announce "Compiling WolfSSL TLS native for SGX"
./build-native-sgx.sh