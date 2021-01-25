#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo "Illegal number of parameters. Please refer to the arguments of the benchmark app."
    exit 1
fi

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly REPO_DIR=$SCRIPT_DIR/../..
readonly RESULTS_DIR=$SCRIPT_DIR/results
readonly DATABASE_TYPE=$1

echo "> Native: Runtime initialization; arguments: $@"
$SCRIPT_DIR/launch-time/native/run.sh "$@" >> $RESULTS_DIR/launch-time-native-$DATABASE_TYPE.csv

echo "> SGX: Runtime initialization; arguments: $@"
$SCRIPT_DIR/launch-time/sgx/run.sh "$@" >> $RESULTS_DIR/launch-time-sgx-$DATABASE_TYPE.csv

echo "> Wasm: Runtime initialization; arguments: $@"
$SCRIPT_DIR/launch-time/wasm/run.sh "$@" >> $RESULTS_DIR/launch-time-wasm-$DATABASE_TYPE.csv

echo "> Wasm-SGX: Runtime initialization; arguments: $@"
$SCRIPT_DIR/launch-time/wasm-sgx/run.sh "$@" >> $RESULTS_DIR/launch-time-wasm-sgx-$DATABASE_TYPE.csv