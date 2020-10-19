#!/bin/bash
readonly ROOT_DIR=$(dirname $(realpath $0))
readonly DATABASE_IN_MEMORY=0
readonly DATABASE_IN_FILE=1

mkdir -p ${ROOT_DIR}/results

# Prototype: benchmark <name-of-profile> <folder-name-with-executable> <executable-name>
function benchmark {
    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        cd ${ROOT_DIR}/$1/$2

        for i in {1..20}
        do
            echo "./$3 $database_type &>> ${ROOT_DIR}/results/$1.csv.."
            ./$3 $database_type &>> ${ROOT_DIR}/results/$1.csv
            if [ ${database_type} == $DATABASE_IN_FILE ]; then
                rm -f testdb testdb_recovery
            fi
        done 
    done
}

echo "Script started at `date`."

benchmark benchmark-native build benchmark-native
benchmark benchmark-wasm build benchmark-wasm
benchmark benchmark-wasm-sgx src app

echo "Script ended at `date`."