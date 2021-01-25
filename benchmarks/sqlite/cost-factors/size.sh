#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo "Illegal number of parameters. Please refer to the arguments of the benchmark app."
    exit 1
fi

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly RESULTS_DIR=$SCRIPT_DIR/results
readonly DATABASE_TYPE=$1

NATIVE=$($SCRIPT_DIR/sizes/native.sh)
SGX_LKL_BIN=$($SCRIPT_DIR/sizes/sgx-lkl-bin.sh)
SGX_LKL_ENCLAVE=$($SCRIPT_DIR/sizes/sgx-lkl-enclave.sh)
SGX_DISK_IMAGE=$($SCRIPT_DIR/sizes/sgx-disk-image.sh)
WASM_AOT=$($SCRIPT_DIR/sizes/wasm-aot.sh)
WASM_BIN=$($SCRIPT_DIR/sizes/wasm-bin.sh)
WASM_SGX_BIN=$($SCRIPT_DIR/sizes/wasm-sgx-bin.sh)
WASM_SGX_ENCLAVE=$($SCRIPT_DIR/sizes/wasm-sgx-enclave.sh)
WASM=$($SCRIPT_DIR/sizes/wasm.sh)

echo "> Native: computing size; arguments: $@"
echo $NATIVE >> $RESULTS_DIR/size-native-$DATABASE_TYPE.csv

echo "> SGX-LKL bin: computing size; arguments: $@"
echo $SGX_LKL_BIN >> $RESULTS_DIR/size-sgx-lkl-bin-$DATABASE_TYPE.csv

echo "> SGX-LKL enclave: computing size; arguments: $@"
echo $SGX_LKL_ENCLAVE >> $RESULTS_DIR/size-sgx-lkl-enclave-$DATABASE_TYPE.csv

echo "> SGX-LKL disk image: computing size; arguments: $@"
echo $SGX_DISK_IMAGE >> $RESULTS_DIR/size-sgx-disk-image-$DATABASE_TYPE.csv

echo "> Wasm AOT: computing size; arguments: $@"
echo $WASM_AOT >> $RESULTS_DIR/size-wasm-aot-$DATABASE_TYPE.csv

echo "> Wasm bin: computing size; arguments: $@"
echo $WASM_BIN >> $RESULTS_DIR/size-wasm-bin-$DATABASE_TYPE.csv

echo "> Wasm SGX bin: computing size; arguments: $@"
echo $WASM_SGX_BIN >> $RESULTS_DIR/size-wasm-sgx-bin-$DATABASE_TYPE.csv

echo "> Wasm SGX enclave: computing size; arguments: $@"
echo $WASM_SGX_ENCLAVE >> $RESULTS_DIR/size-wasm-sgx-enclave-$DATABASE_TYPE.csv

echo "> Wasm: computing size; arguments: $@"
echo $WASM >> $RESULTS_DIR/size-wasm-$DATABASE_TYPE.csv