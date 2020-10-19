#!/bin/bash
# The first argument is forwarded to the environment variable BUILDING_BENCHMARK_FLAG
# to Docker containers.

ROOT_DIR=$(dirname $(realpath $0))

WAMR_UNINE_PATH="/opt/wamr-sdk/"
POLYBENCHC_NATIVE_DIR=$ROOT_DIR/polybench/dist/polybench-c
POLYBENCHC_WASM_DIR=$ROOT_DIR/polybench/dist/webassembly-polybench-c

# exit when any command fails
set -e

# Set up the git repositories that are the dependencies of the project
if [ ! -d "$WAMR_UNINE_PATH" ]; then
  echo "TWINE runtime not found ($WAMR_UNINE_PATH)."
  exit 1
fi

cd $WAMR_UNINE_PATH
git pull

cd $ROOT_DIR


# Build the environment for compiling the benchmarks
docker build -t unine_benchmark_linux-sgx ./linux-sgx
docker build -t unine_benchmark_polybench_sgx ./polybench
docker build -t polybenchc_wasm_compiler ./polybench/dist/webassembly-polybench-c/utilities/compile-wasm

# Compile the benchmarks
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v `pwd`/linux-sgx:/linux-sgx \
    -e BUILDING_SDK_FLAG=$1 \
    unine_benchmark_linux-sgx

# Compile the benchmarks
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v `pwd`/linux-sgx:/linux-sgx \
    -v `pwd`/polybench:/polybenchc \
    unine_benchmark_polybench_sgx


# Build the native benchmark experiments
echo "[+] Building polybench/c native"
rm -rf $ROOT_DIR/dist/polybench-c/output
git submodule update --init
cd $POLYBENCHC_NATIVE_DIR

docker build . -t polybenchc_builder
docker run \
  --rm \
  -u $(id -u ${USER}):$(id -g ${USER}) \
  -v `pwd`:/polybench \
  polybenchc_builder


# Build the WebAssembly benchmark experiments
echo "[+] Building polybench/c web assemblies"

cd $POLYBENCHC_WASM_DIR/utilities/compile-wasm
docker run \
    --rm \
    -v $POLYBENCHC_WASM_DIR:/polybenchc \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    polybenchc_wasm_compiler \
    --aot \
    --sgx \
    --output /polybenchc/output