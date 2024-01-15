#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
DIST_POLYBENCH=$SCRIPT_DIR/dist/webassembly-polybench-c
source $SCRIPT_DIR/../common.sh

announce "Compiling Polybench/C Wasm"

docker build $DIST_POLYBENCH/utilities/compile-wasm -t webassembly_polybench
docker run \
    --rm \
    -v $DIST_POLYBENCH:/polybenchc \
    -v /opt/wamr:/wamr \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    webassembly_polybench \
    --dataset-size LARGE_DATASET \
    --output /polybenchc/output \
    --display-time \
    --aot-bounds-checks 0 \
    --wamr /wamr \
    --aot \
    --sgx