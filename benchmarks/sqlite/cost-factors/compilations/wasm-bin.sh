#!/bin/bash

ROOT_DIR=/sqlite/benchmark-wasm

mkdir -p ${ROOT_DIR}/build
cd ${ROOT_DIR}/build
cmake ..
make