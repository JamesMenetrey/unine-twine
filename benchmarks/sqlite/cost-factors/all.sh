#!/bin/bash

set -e

readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly DB_SIZE=175000
readonly DATABASE_IN_MEMORY=0
readonly DATABASE_IN_FILE=1
readonly PRINT_MEMORY_USAGE=0
readonly PROFILING_LEVEL=0
readonly NUMBER_OF_ITERATIONS=10

# Prototype: evaluate-costs
function evaluate-costs {
    for database_type in $DATABASE_IN_MEMORY $DATABASE_IN_FILE
    do
        for i in $(seq 1 $NUMBER_OF_ITERATIONS)
        do
            $SCRIPT_DIR/compilation.sh $database_type $DB_SIZE $DB_SIZE 0 0
            $SCRIPT_DIR/optimization.sh $database_type $DB_SIZE $DB_SIZE 0 0
            $SCRIPT_DIR/launch-time-build.sh
            $SCRIPT_DIR/launch-time-run.sh $database_type $DB_SIZE $DB_SIZE 0 0
            $SCRIPT_DIR/resident-memory-build.sh
            $SCRIPT_DIR/resident-memory-run.sh $database_type $DB_SIZE $DB_SIZE 0 0
            $SCRIPT_DIR/size.sh $database_type $DB_SIZE $DB_SIZE 0 0
        done
    done
}

echo "Script started at `date`."

evaluate-costs

echo "Script ended at `date`."