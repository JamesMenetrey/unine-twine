#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo "Illegal number of parameters. Please refer to the arguments of the benchmark app."
    exit 1
fi

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly REPO_DIR=$SCRIPT_DIR/../..
readonly RESULTS_DIR=$SCRIPT_DIR/results
readonly DATABASE_TYPE=$1

TIMEFORMAT=%R

echo "> Wasm: Optimization — AoT (*.wasm → *.aot); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/optimizations/wasm-aot-cleanup.sh

( time \
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/optimizations/wasm-aot.sh
) 2>> $RESULTS_DIR/optimization-wasm-aot-$DATABASE_TYPE.csv

echo "> SGX: Creation of disk image (*.so → disk image); arguments: $@"

$SCRIPT_DIR/optimizations/sgx-disk-image-cleanup.sh

( time \
$SCRIPT_DIR/optimizations/sgx-disk-image.sh
) 2>> $RESULTS_DIR/optimization-sgx-disk-image-$DATABASE_TYPE.csv
