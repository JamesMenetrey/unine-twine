#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

set -x -e

cd $SCRIPT_DIR/../../../benchmark-wasm-sgx/
mkdir -p build
cd build
cmake ..
make

cd $SCRIPT_DIR/../../../benchmark-wasm-sgx/src
make clean
make INSTRUMENTATION_CALLBACKS_NAME=memory_callbacks

rm -rf $SCRIPT_DIR/build
mkdir -p $SCRIPT_DIR/build
cp {app,enclave.signed.so,benchmark-wasm.aot} $SCRIPT_DIR/build