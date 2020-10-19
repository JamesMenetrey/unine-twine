#!/bin/bash
# The first argument is forwarded to the environment variable BUILDING_BENCHMARK_FLAG
# to Docker containers.

ROOT_DIR=$(dirname $(realpath $0))

# exit when any command fails
set -e

# Set up the git repositories that are the dependencies of the project
WAMR_UNINE_PATH="/opt/wamr-sdk/"
WAMR_OFFICIAL_PATH="/opt/wamr-sdk-official/"

if [ ! -d "$WAMR_UNINE_PATH" ]; then
  echo "TWINE runtime not found ($WAMR_UNINE_PATH)."
  exit 1
fi

cd $WAMR_UNINE_PATH
git pull

if [ ! -d "$WAMR_OFFICIAL_PATH" ]; then
  echo "WAMR not found ($WAMR_OFFICIAL_PATH)."
  exit 1
fi

cd $WAMR_OFFICIAL_PATH
git pull

cd $ROOT_DIR

# Build the environment for compiling the benchmarks
docker build -t unine_benchmark_linux-sgx ./linux-sgx
docker build -t unine_benchmark_sqlite ./sqlite

# Compile the benchmarks
docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v `pwd`/linux-sgx:/linux-sgx \
    -e BUILDING_SDK_FLAG=$1 \
    unine_benchmark_linux-sgx

docker run \
    --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v /opt/wamr-sdk:/opt/wamr-sdk \
    -v /opt/wamr-sdk-official:/opt/wamr-sdk-official \
    -v `pwd`/linux-sgx:/linux-sgx \
    -v `pwd`/sqlite:/sqlite \
    -e BUILDING_BENCHMARK_FLAG=$1 \
    unine_benchmark_sqlite