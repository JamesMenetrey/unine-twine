#!/bin/bash

ROOT_DIR=/sqlite/benchmark-wasm-sgx

cd $ROOT_DIR/build
cmake ..
make