#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

set -e

cd $SCRIPT_DIR/build
./app "$@"
rm -f benchmark.db benchmark.db_recovery