#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

# Display the executed commands and stop upon an error occurs
set -x -e

# Define path variables
DOCKER_SQLITE_IMAGE=unine_benchmark_sgx_lkl_sqlite_memory
SGX_LK_IMAGE=$ROOT_DIR/sgx-lkl-sqlite.img

# Remove any previous SGX-LKL disk image
rm -f $SGX_LK_IMAGE

# Build the Docker image for SQLite running in SGX
docker build --build-arg INSTRUMENTATION_CALLBACKS=memory_callbacks_external -f $ROOT_DIR/../../../benchmark-sgx/Dockerfile -t $DOCKER_SQLITE_IMAGE $ROOT_DIR/../../..

# Create the SGX-LKL disk image based on the previously created Docker image
# The size is set to 250MB, similarly to the size set to the SGX enclave configuration with TWINE (HeapMaxSize)
/opt/sgx-lkl-nonrelease/bin/sgx-lkl-disk create --size=250M --docker=$DOCKER_SQLITE_IMAGE --encrypt --integrity --key-file $SGX_LK_IMAGE