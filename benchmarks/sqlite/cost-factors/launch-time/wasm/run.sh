#!/bin/bash

set -e

readonly SCRIPT_DIR=$(dirname $(realpath $0))

cd $SCRIPT_DIR/build/benchmark-wasm
python3 $SCRIPT_DIR/../time_estimation.py ./benchmark-wasm "$@"