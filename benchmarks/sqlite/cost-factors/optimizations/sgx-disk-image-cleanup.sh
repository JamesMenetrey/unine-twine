#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))/../../
BENCHMARK_SGX=$ROOT_DIR/benchmark-sgx
DOCKER_SQLITE_IMAGE=unine_benchmark_sgx_lkl_sqlite

rm -f $BENCHMARK_SGX/sgx-lkl-sqlite.*

# Build the Docker image for SQLite running in SGX
docker build -f $BENCHMARK_SGX/Dockerfile -t $DOCKER_SQLITE_IMAGE $ROOT_DIR