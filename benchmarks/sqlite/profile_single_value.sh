#!/bin/bash
readonly ROOT_DIR=$(dirname $(realpath $0))
readonly DATABASE_IN_MEMORY=0
readonly DATABASE_IN_FILE=1
readonly PRINT_MEMORY_USAGE=0

set -e

if [ ! $# -eq 1 ] ; then
    echo "usage: number_of_records"
    exit 1
fi

# Profiling levels
readonly PROFILING_LEVEL_NONE=0
readonly PROFILING_TRUSTED_LEVEL_ATOMIC=1001
readonly PROFILING_TRUSTED_LEVEL_FILE_API=1002
readonly PROFILING_TRUSTED_LEVEL_GET_DATA_NODE=1003
readonly PROFILING_TRUSTED_LEVEL_APPEND_DATA_NODE=1004
readonly PROFILING_TRUSTED_LEVEL_READ_DATA_NODE=1005
readonly PROFILING_TRUSTED_LEVEL_GET_MHT_NODE=1006
readonly PROFILING_TRUSTED_LEVEL_READ_MHT_NODE=1007
readonly PROFILING_TRUSTED_LEVEL_APPEND_MHT_NODE=1008
readonly PROFILING_TRUSTED_LEVEL_INTERNAL_FLUSH=1009
readonly PROFILING_UNTRUSTED_LEVEL_FILE_API=2001
readonly PROFILING_WASI_LEVEL_WASI_API=3001
readonly PROFILING_ALL_LEVELS="$PROFILING_LEVEL_NONE $PROFILING_TRUSTED_LEVEL_ATOMIC $PROFILING_TRUSTED_LEVEL_FILE_API \
    $PROFILING_TRUSTED_LEVEL_GET_DATA_NODE $PROFILING_TRUSTED_LEVEL_APPEND_DATA_NODE $PROFILING_TRUSTED_LEVEL_READ_DATA_NODE \
    $PROFILING_TRUSTED_LEVEL_GET_MHT_NODE $PROFILING_TRUSTED_LEVEL_READ_MHT_NODE $PROFILING_TRUSTED_LEVEL_APPEND_MHT_NODE \
    $PROFILING_TRUSTED_LEVEL_INTERNAL_FLUSH $PROFILING_UNTRUSTED_LEVEL_FILE_API $PROFILING_WASI_LEVEL_WASI_API"

mkdir -p ${ROOT_DIR}/results

# Prototype: profile <name-of-profile> <folder-name-with-executable> <executable-name> <number-of-records>
function profile {
    cd ${ROOT_DIR}/$1/$2

    for profiling_level in $PROFILING_ALL_LEVELS
    do
        for i in {1..5}
        do
            echo "./$3 $DATABASE_IN_FILE $4 $4 $PRINT_MEMORY_USAGE $profiling_level &>> ${ROOT_DIR}/results/profiling-$1.csv.."
            ./$3 $DATABASE_IN_FILE $4 $4 $PRINT_MEMORY_USAGE $profiling_level &>> ${ROOT_DIR}/results/profiling-$1.csv
            rm -f benchmark.db benchmark.db_recovery benchmark.db-journal benchmark.db-journal_recovery
        done
    done
}

echo "Script started at `date`."

profile benchmark-wasm-sgx src app $1

echo "Script ended at `date`."