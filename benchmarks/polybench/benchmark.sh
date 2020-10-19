#!/bin/bash
readonly ROOT_DIR=$(dirname $(realpath $0))
readonly POLYBENCHC_NATIVE_FILES=$ROOT_DIR/dist/polybench-c/output/*
readonly POLYBENCHC_AOT_FILES=$ROOT_DIR/dist/webassembly-polybench-c/output/*.aot
readonly ITERATION_NUMBER=50

mkdir -p ${ROOT_DIR}/results

# Prototype: benchmark_native
function benchmark_native {
    for polybench_file in $POLYBENCHC_NATIVE_FILES
    do
        for i in $(eval echo "{1..$ITERATION_NUMBER}")
        do
            echo "$polybench_file &>> ${ROOT_DIR}/results/benchmark-native-$(basename ${polybench_file}).csv.."
            $polybench_file &>> ${ROOT_DIR}/results/benchmark-native-$(basename ${polybench_file}).csv
        done
    done
}

# Prototype: benchmark_wasm <name-of-profile> <folder-name-with-executable> <executable-name>
function benchmark_wasm {
    for polybench_file in $POLYBENCHC_AOT_FILES
    do
        cd ${ROOT_DIR}/$1/$2
        for i in $(eval echo "{1..$ITERATION_NUMBER}")
        do
            echo "./$3 ${polybench_file} &>> ${ROOT_DIR}/results/$1-$(basename ${polybench_file} .aot).csv.."
            ./$3 ${polybench_file} &>> ${ROOT_DIR}/results/$1-$(basename ${polybench_file} .aot).csv
        done 
    done
}

echo "Script started at `date`."

benchmark_native
benchmark_wasm benchmark-wasm build benchmark-wasm
benchmark_wasm benchmark-wasm-sgx src app

echo "Script ended at `date`."