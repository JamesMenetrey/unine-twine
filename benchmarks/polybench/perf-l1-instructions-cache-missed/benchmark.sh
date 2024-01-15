#!/bin/bash
readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly POLYBENCHC_NATIVE_FILES=$SCRIPT_DIR/dist/polybench-c/output/*
readonly POLYBENCHC_EMCC_FILES=$SCRIPT_DIR/dist/emcc-polybench-c/output/*.js
readonly POLYBENCHC_AOT_FILES=$SCRIPT_DIR/dist/webassembly-polybench-c/output/*.aot
readonly ITERATION_NUMBER=50

source $SCRIPT_DIR/../common.sh

mkdir -p ${SCRIPT_DIR}/results

# Prototype: benchmark_native
function benchmark_native {
    PROFILE_NAME=$1

    for polybench_file in $POLYBENCHC_NATIVE_FILES
    do
        for i in $(eval echo "{1..$ITERATION_NUMBER}")
        do
            echo "$polybench_file &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME-$(basename ${polybench_file}).csv.."
            $polybench_file &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME-$(basename ${polybench_file}).csv
        done
    done
}

# Prototype: benchmark_wasm_sgx_nodejs
function benchmark_wasm_sgx_nodejs {
    cd ${SCRIPT_DIR}/benchmark-sgx-nodejs

    for polybench_path in $POLYBENCHC_EMCC_FILES
    do
        for i in $(eval echo "{1..$ITERATION_NUMBER}")
        do
            polybench_file=$(basename "$polybench_path")
            polybench_file_without_extension="${polybench_file%.*}"

            echo "./run-benchmark.sh $polybench_file &>> ${SCRIPT_DIR}/results/benchmark-wasm-sgx-nodejs-${polybench_file_without_extension}.csv.."
            ./run-benchmark.sh $polybench_file &>> ${SCRIPT_DIR}/results/benchmark-wasm-sgx-nodejs-${polybench_file_without_extension}.csv
        done 
    done
}

# Prototype: benchmark_wasm <profile_name> <iwasm_path>
function benchmark_wasm {
    PROFILE_NAME=$1
    IWASM_PATH=$2

    for polybench_file in $POLYBENCHC_AOT_FILES
    do
        for i in $(eval echo "{1..$ITERATION_NUMBER}")
        do
            echo "$IWASM_PATH --stack-size=$((5 * 1024 * 1024)) --heap-size=$((4 * 1024 * 1024)) ${polybench_file} &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME-$(basename ${polybench_file} .aot).csv"
            $IWASM_PATH --stack-size=$((5 * 1024 * 1024)) --heap-size=$((4 * 1024 * 1024)) ${polybench_file} &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME-$(basename ${polybench_file} .aot).csv
        done 
    done
}

echo "Script started at `date`."

disable_large_pages
benchmark_native native
benchmark_wasm wasm-no-large-pages $WAMR_PATH/product-mini/platforms/linux/build/iwasm

enable_large_pages
benchmark_native native-large-pages
benchmark_wasm wasm $WAMR_PATH/product-mini/platforms/linux/build/iwasm
benchmark_wasm wasm-sgx $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample/iwasm
benchmark_wasm_sgx_nodejs

# Clean up
optin_large_pages

echo "Script ended at `date`."