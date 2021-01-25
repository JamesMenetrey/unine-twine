#!/bin/bash
readonly ROOT_DIR=$(dirname $(realpath $0))
readonly DATABASE_IN_MEMORY=0
readonly DATABASE_IN_FILE=1
readonly PRINT_MEMORY_USAGE=0
readonly PROFILING_LEVEL=0

mkdir -p ${ROOT_DIR}/results

# Prototype: benchmark <name-of-profile> <folder-name-with-executable> <executable-name>
function benchmark {
    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${ROOT_DIR}/$1/$2

        for size in {1000..175000..1000}
        do
            for i in {1..10}
            do
                echo "./$3 $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${ROOT_DIR}/results/$1.csv.."
                ./$3 $database_type $size $size $PRINT_MEMORY_USAGE $PROFILING_LEVEL &>> ${ROOT_DIR}/results/$1.csv
                if [ ${database_type} == $DATABASE_IN_FILE ]; then
                    rm -f benchmark.db benchmark.db_recovery benchmark.db-journal benchmark.db-journal_recovery
                fi
            done
        done
        
    done
}

echo "Script started at `date`."

benchmark benchmark-native build benchmark-native
benchmark benchmark-wasm build benchmark-wasm
benchmark benchmark-sgx . run-benchmark.sh
benchmark benchmark-wasm-sgx src app

# Enable to measure the official WAMR implementation
#benchmark benchmark-wasm-sgx-official src app

echo "Script ended at `date`."