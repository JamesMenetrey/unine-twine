#!/bin/bash

set -e

readonly SCRIPT_DIR=$(dirname $(realpath $0))

cd $SCRIPT_DIR/build/benchmark-native
./benchmark-native "$@"
rm -f benchmark.db