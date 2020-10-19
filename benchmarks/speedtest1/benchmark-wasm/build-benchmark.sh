#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

WAMR_ROOT_DIR=/opt/wamr-sdk

# Check WASI SDK is present
if [ ! -d $WAMR_ROOT_DIR ] 
then
    echo "Error: WAMR SDK not located into $WAMR_ROOT_DIR."
    echo "Stopping building benchmark-wasm."
    exit 1
fi

mkdir -p ${ROOT_DIR}/build
cd ${ROOT_DIR}/build
cmake ..
make