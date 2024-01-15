#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
DIST_POLYBENCH=$SCRIPT_DIR/dist/polybench-c
source $SCRIPT_DIR/../common.sh

announce "Compiling Polybench/C native"

cd $DIST_POLYBENCH
rm -rf output
docker build . -t polybenchc_builder
docker run \
  --rm \
  -u $(id -u ${USER}):$(id -g ${USER}) \
  -v $DIST_POLYBENCH:/polybench \
  polybenchc_builder