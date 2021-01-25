#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

# Display the executed commands
set -x

# Define path variables
SQLITE_ROOT=$ROOT_DIR/../dist/sqlite
SQLITE_ROOT_LN=$ROOT_DIR/sqlite

BENCHMARK_ROOT=$ROOT_DIR/../benchmark
BENCHMARK_ROOT_LN=$ROOT_DIR/benchmark

DOCKER_SQLITE_IMAGE=unine_benchmark_sgx_lkl_sqlite
SGX_LK_IMAGE=$ROOT_DIR/sgx-lkl-sqlite.img

# Build the Docker image for SQLite running in SGX
docker build -f $ROOT_DIR/Dockerfile -t $DOCKER_SQLITE_IMAGE $ROOT_DIR/..

# Remove any previous SGX-LKL disk image
rm -f $SGX_LK_IMAGE

# Create the SGX-LKL disk image based on the previously created Docker image
# The size is set to 250MB, similarly to the size set to the SGX enclave configuration with TWINE (HeapMaxSize)
/opt/sgx-lkl-nonrelease/bin/sgx-lkl-disk create --size=205M --docker=$DOCKER_SQLITE_IMAGE --encrypt --integrity --key-file $SGX_LK_IMAGE