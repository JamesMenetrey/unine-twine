#!/bin/bash
readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly DATABASE_IN_MEMORY=0
readonly DATABASE_IN_FILE=1
readonly PRINT_MEMORY_USAGE=0
readonly PROFILING_LEVEL=0

source $SCRIPT_DIR/../common.sh

mkdir -p ${SCRIPT_DIR}/results

# Prototype: benchmark <profile_name> <app_path>
function benchmark_native {
    PROFILE_NAME=$1
    APP_PATH=$2

    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${SCRIPT_DIR}/build/native

        for size in {1000..175000..1000}
        do
            for i in {1..10}
            do
                echo "./$APP_PATH $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
                ./$APP_PATH $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv
                if [ ${database_type} == $DATABASE_IN_FILE ]; then
                    rm -f benchmark.db benchmark.db_recovery benchmark.db-journal benchmark.db-journal_recovery
                fi
            done
        done
        
    done
}

# Prototype: benchmark_native_sgx
function benchmark_native_sgx {
    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${SCRIPT_DIR}/benchmark-sgx

        for size in {1000..175000..1000}
        do
            for i in {1..10}
            do
                echo "./run-benchmark.sh $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv.."
                ./run-benchmark.sh $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv
                if [ ${database_type} == $DATABASE_IN_FILE ]; then
                    rm -f benchmark.db benchmark.db_recovery benchmark.db-journal benchmark.db-journal_recovery
                fi
            done
        done
        
    done
}

# Prototype: benchmark_wasm <profile_name> <iwasm_path>
function benchmark_wasm {
    PROFILE_NAME=$1
    IWASM_PATH=$2

    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${SCRIPT_DIR}/build/wasm

        for size in {1000..175000..1000}
        do
            for i in {1..10}
            do
                echo "$IWASM_PATH --stack-size=$((1 * 1024 * 1024)) --heap-size=$((0 * 1024 * 1024)) --dir=. benchmark-wasm.aot $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
                $IWASM_PATH --stack-size=$((1 * 1024 * 1024)) --heap-size=$((0 * 1024 * 1024)) --dir=. benchmark-wasm.aot $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv
                if [ ${database_type} == $DATABASE_IN_FILE ]; then
                    rm -f benchmark.db benchmark.db_recovery benchmark.db-journal benchmark.db-journal_recovery
                fi
            done
        done
        
    done
}

echo "Script started at `date`."

disable_large_pages
benchmark_native native benchmark

enable_large_pages
benchmark_native native-large-pages benchmark
benchmark_wasm wasm $WAMR_PATH/product-mini/platforms/linux/build/iwasm
benchmark_native_sgx
benchmark_wasm wasm-sgx $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample/iwasm

# Clean up
optin_large_pages

echo "Script ended at `date`."