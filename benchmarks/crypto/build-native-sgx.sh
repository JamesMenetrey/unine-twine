#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

# Display the executed commands
set -x

# Define path variables
DOCKER_IMAGE=unine_benchmark_sgx_lkl_wolfssl_crypto
BENCHMARK_DIR=native-sgx
SGX_LK_IMAGE=$SCRIPT_DIR/$BENCHMARK_DIR/sgx-lkl-wolfssl-crypto.img

# Remove any previous SGX-LKL disk image
sudo rm -f $SGX_LK_IMAGE*

# Build the Docker image running in SGX
docker build -f $SCRIPT_DIR/$BENCHMARK_DIR/Dockerfile -t $DOCKER_IMAGE $SCRIPT_DIR/..

# Create the SGX-LKL disk image based on the previously created Docker image
# The size is set to 80MB, similarly to the size set to the SGX enclave configuration with TWINE (HeapMaxSize)
/opt/sgx-lkl-nonrelease/bin/sgx-lkl-disk create --size=80M --docker=$DOCKER_IMAGE --encrypt --integrity --key-file $SGX_LK_IMAGE
