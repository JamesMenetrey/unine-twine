#!/bin/bash

readonly SCRIPT_DIR=$(dirname $(realpath $0))

rm -rf $SCRIPT_DIR/build
mkdir -p $SCRIPT_DIR/build
cd $SCRIPT_DIR/build
cmake ..
make