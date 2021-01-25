#!/bin/bash

readonly SCRIPT_DIR=$(dirname $(realpath $0))

set -x -e

rm -rf $SCRIPT_DIR/build
mkdir -p $SCRIPT_DIR/build
cd $SCRIPT_DIR/build
cmake ..
make

cp $SCRIPT_DIR/../../../benchmark-wasm/build/benchmark-wasm.aot benchmark-wasm