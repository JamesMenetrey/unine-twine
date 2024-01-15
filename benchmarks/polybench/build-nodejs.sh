#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
DIST_POLYBENCH=$SCRIPT_DIR/dist/emcc-polybench-c
source $SCRIPT_DIR/../common.sh

announce "Compiling Polybench/C Emscripten"

readonly POLYBENCH_BUILD_PATH=$DIST_POLYBENCH/output
readonly LKL_BUILD_PATH=$SCRIPT_DIR/benchmark-sgx-nodejs
readonly EMCC_BUILD_PATH=$LKL_BUILD_PATH/output

cd $DIST_POLYBENCH
rm -rf $POLYBENCH_BUILD_PATH
docker build . -t emcc_polybenchc_builder
docker run \
  --rm \
  -u $(id -u ${USER}):$(id -g ${USER}) \
  -v $DIST_POLYBENCH:/polybench \
  emcc_polybenchc_builder

cd $SCRIPT_DIR
rm -f $EMCC_BUILD_PATH
mkdir -p $EMCC_BUILD_PATH
cp $POLYBENCH_BUILD_PATH/* $EMCC_BUILD_PATH
touch $EMCC_BUILD_PATH

# We exclude running the benchmaarks Deriche for NodeJS, as it leads to a seg fault.
# Note that the benchmark of AccTEE reduces the dimension of Deriche, which but Twine supports
# running it with the default large size.
rm $EMCC_BUILD_PATH/deriche.*

SGXLKL_MMAP_FILES=Shared SGXLKL_VERBOSE=0 SGXLKL_KERNEL_VERBOSE=0 \
    SGXLKL_TRACE_SIGNAL=0 SGXLKL_TRACE_HOST_SYSCALL=0 SGXLKL_TRACE_LKL_SYSCALL=0 SGXLKL_TRACE_MMAP=0 \
    sgx-lkl-disk create -f --size=2G --docker=${LKL_BUILD_PATH}/Dockerfile ${LKL_BUILD_PATH}/sgxlkl-nodejs.img
