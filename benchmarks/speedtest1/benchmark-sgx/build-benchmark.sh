#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

# Display the executed commands
set -x

# Define path variables
DOCKER_SQLITE_IMAGE=unine_benchmark_sgx_lkl_speedtest1
SGX_LK_IMAGE=$SCRIPT_DIR/sgx-lkl-speedtest1.img

# Build the Docker image for SQLite running in SGX
docker build -f $SCRIPT_DIR/Dockerfile -t $DOCKER_SQLITE_IMAGE $SCRIPT_DIR/../..

# Remove any previous SGX-LKL disk image
rm -f $SGX_LK_IMAGE

# Create the SGX-LKL disk image based on the previously created Docker image
# The size is set to 60MB, similarly to the size set to the SGX enclave configuration with TWINE (HeapMaxSize)
/opt/sgx-lkl-nonrelease/bin/sgx-lkl-disk create --size=60M --docker=$DOCKER_SQLITE_IMAGE --encrypt --integrity --key-file $SGX_LK_IMAGE
