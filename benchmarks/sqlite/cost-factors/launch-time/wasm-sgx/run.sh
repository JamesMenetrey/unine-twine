#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

set -e

cd $SCRIPT_DIR/build
python3 $SCRIPT_DIR/../time_estimation.py ./app "$@"