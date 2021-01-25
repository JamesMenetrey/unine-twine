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


echo "> SGX-LKL: Compilation time runtime; arguments: $@"
( time \
    $SCRIPT_DIR/compilations/sgx/run.sh  2>> $RESULTS_DIR/warnings-sgx.log | tee $RESULTS_DIR/output-sgx.log
) 2>> $RESULTS_DIR/compilation-sgx-$DATABASE_TYPE.csv


echo "> Wasm-bin: Compilation time runtime (GCC; *.c → *.so); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/wasm-bin-cleanup.sh

( time \
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/wasm-bin.sh 2>> $RESULTS_DIR/warnings-wasm-bin.log
) 2>> $RESULTS_DIR/compilation-wasm-bin-$DATABASE_TYPE.csv


echo "> Wasm-sgx-bin: Compilation time runtime (*.c → *.so); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/wasm-sgx-bin-cleanup.sh

( time \
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/wasm-sgx-bin.sh 2>> $RESULTS_DIR/warnings-wasm-sgx-bin.log
) 2>> $RESULTS_DIR/compilation-wasm-sgx-bin-$DATABASE_TYPE.csv


echo "> Native: Compilation time (*.c → *.so); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/native-cleanup.sh

( time \
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/native.sh
) 2>> $RESULTS_DIR/compilation-native-$DATABASE_TYPE.csv


echo "> Wasm: Compilation time SQLite (*.c → *.wasm); arguments: $@"
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/wasm-cleanup.sh

( time \
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $REPO_DIR/sqlite:/sqlite \
    unine_benchmark_sqlite \
    /sqlite/cost-factors/compilations/wasm.sh
) 2>> $RESULTS_DIR/compilation-wasm-$DATABASE_TYPE.csv