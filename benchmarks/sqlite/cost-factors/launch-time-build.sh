#!/bin/bash

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly REPO_DIR=$SCRIPT_DIR/../..
readonly RESULTS_DIR=$SCRIPT_DIR/results

echo "> Native: Runtime initialization (building); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/launch-time/native/build.sh

echo "> SGX: Runtime initialization (building); arguments: $@"
$SCRIPT_DIR/launch-time/sgx/build.sh

echo "> Wasm: Runtime initialization (building); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/launch-time/wasm/build.sh

echo "> Wasm-SGX: Runtime initialization (building); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/linux-sgx:/linux-sgx \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/launch-time/wasm-sgx/build.sh