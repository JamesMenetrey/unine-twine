#!/bin/bash
readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly DATABASE_IN_MEMORY=0
readonly DATABASE_IN_FILE=1

source $SCRIPT_DIR/../common.sh

mkdir -p ${SCRIPT_DIR}/results

# Prototype: benchmark_native <profile_name>
function benchmark_native {
    PROFILE_NAME=$1

    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${SCRIPT_DIR}/build/native

        for i in {1..25}
        do
            echo "./benchmark $database_type &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
            ./benchmark $database_type &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv
            if [ ${database_type} == $DATABASE_IN_FILE ]; then
                rm -f testdb testdb_recovery testdb-journal testdb-journal_recovery
            fi
        done 
    done
}

# Prototype: benchmark_native_sgx
function benchmark_native_sgx {
    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${SCRIPT_DIR}/benchmark-sgx

        for i in {1..25}
        do
            echo "./run-benchmark.sh $database_type &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv.."
            ./run-benchmark.sh $database_type &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv
        done 
    done
}

# Prototype: benchmark <profile_name> <iwasm_path>
function benchmark_wasm {
    PROFILE_NAME=$1
    IWASM_PATH=$2

    cd ${SCRIPT_DIR}/build/wasm

    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        for i in {1..25}
        do
            echo "$IWASM_PATH --stack-size=$((5 * 1024 * 1024)) --heap-size=$((4 * 1024 * 1024)) --dir=. benchmark-wasm.aot $database_type &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
            $IWASM_PATH --stack-size=$((5 * 1024 * 1024)) --heap-size=$((4 * 1024 * 1024)) --dir=. benchmark-wasm.aot $database_type &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv
            if [ ${database_type} == $DATABASE_IN_FILE ]; then
                rm -f testdb testdb_recovery testdb-journal testdb-journal_recovery
            fi
        done 
    done
}

echo "Script started at `date`."

disable_large_pages
benchmark_native native-no-large-pages
benchmark_wasm wasm-no-large-pages $WAMR_PATH/product-mini/platforms/linux/build/iwasm

enable_large_pages
benchmark_native native-large-pages
benchmark_wasm wasm $WAMR_PATH/product-mini/platforms/linux/build/iwasm
benchmark_native_sgx
benchmark_wasm wasm-sgx $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample/iwasm

# Clean up
optin_large_pages

echo "Script ended at `date`."