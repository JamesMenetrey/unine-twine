#!/bin/bash
readonly SCRIPT_DIR=$(dirname $(realpath $0))

source $SCRIPT_DIR/../common.sh

mkdir -p ${SCRIPT_DIR}/results

# Prototype: benchmark_native <profile_name>
function benchmark_native {
    PROFILE_NAME=$1

    cd ${SCRIPT_DIR}/native/build

    for i in {1..100}
    do
        echo "./wasm_example -b &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
        ./wasm_example -b &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv
    done 
}

# Prototype: benchmark_native_sgx
function benchmark_native_sgx {
    cd ${SCRIPT_DIR}/native-sgx

    for i in {1..100}
    do
        echo "./run-benchmark.sh -b &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv.."
        ./run-benchmark.sh -b &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv
    done
}

# Prototype: benchmark <profile_name> <iwasm_path>
function benchmark_wasm {
    PROFILE_NAME=$1
    IWASM_PATH=$2

    cd ${SCRIPT_DIR}/wasm/build

    for i in {1..100}
    do
        echo "$IWASM_PATH --stack-size=$((5 * 1024 * 1024)) --heap-size=$((10 * 1024 * 1024)) wasm_example.aot -b &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
        $IWASM_PATH --stack-size=$((5 * 1024 * 1024)) --heap-size=$((10 * 1024 * 1024)) wasm_example.aot -b &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv
    done 
}

echo "Script started at `date`."

disable_large_pages
benchmark_native native

enable_large_pages
benchmark_native native-large-pages
benchmark_wasm wasm $WAMR_PATH/product-mini/platforms/linux/build/iwasm
benchmark_native_sgx
benchmark_wasm wasm-sgx $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample/iwasm

# Clean up
optin_large_pages

echo "Script ended at `date`."