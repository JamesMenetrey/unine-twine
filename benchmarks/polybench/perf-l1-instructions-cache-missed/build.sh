#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
DIST_POLYBENCH=$SCRIPT_DIR/dist/webassembly-polybench-c
source $SCRIPT_DIR/../common.sh

build_optimized_ipfs
build_runtime $((160 * 1024 * 1024)) $((180 * 1024 * 1024)) $((4 * 1024 * 1024)) $((4 * 1024 * 1024))

$SCRIPT_DIR/build-native.sh
$SCRIPT_DIR/build-wasm.sh