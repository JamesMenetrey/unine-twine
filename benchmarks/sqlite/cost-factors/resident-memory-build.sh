#!/bin/bash

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly REPO_DIR=$SCRIPT_DIR/../..
readonly RESULTS_DIR=$SCRIPT_DIR/results

echo "> Native: Resident memory (building); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/resident-memory/native/build.sh

echo "> SGX: Resident memory (building); arguments: $@"
$SCRIPT_DIR/resident-memory/sgx/build.sh

echo "> Wasm: Resident memory (building); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/resident-memory/wasm/build.sh

echo "> Wasm-SGX: Resident memory (building); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/linux-sgx:/linux-sgx \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/resident-memory/wasm-sgx/build.sh