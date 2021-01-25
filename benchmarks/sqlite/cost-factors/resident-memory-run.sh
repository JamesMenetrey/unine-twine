#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo "Illegal number of parameters. Please refer to the arguments of the benchmark app."
    exit 1
fi

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly REPO_DIR=$SCRIPT_DIR/../..
readonly RESULTS_DIR=$SCRIPT_DIR/results
readonly DATABASE_TYPE=$1

echo "> Native: Resident memory; arguments: $@"
$SCRIPT_DIR/resident-memory/native/run.sh "$@" >> $RESULTS_DIR/resident-memory-native-$DATABASE_TYPE.csv

echo "> SGX: Resident memory; arguments: $@"
$SCRIPT_DIR/resident-memory/sgx/run.sh "$@" >> $RESULTS_DIR/resident-memory-sgx-$DATABASE_TYPE.csv

echo "> Wasm: Resident memory; arguments: $@"
$SCRIPT_DIR/resident-memory/wasm/run.sh "$@" >> $RESULTS_DIR/resident-memory-wasm-$DATABASE_TYPE.csv

echo "> Wasm-SGX: Resident memory; arguments: $@"
$SCRIPT_DIR/resident-memory/wasm-sgx/run.sh "$@" >> $RESULTS_DIR/resident-memory-wasm-sgx-$DATABASE_TYPE.csv