#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))/../../
BENCHMARK_SGX=$ROOT_DIR/benchmark-sgx
SGX_LK_IMAGE=$BENCHMARK_SGX/sgx-lkl-sqlite.img
DOCKER_SQLITE_IMAGE=unine_benchmark_sgx_lkl_sqlite

/opt/sgx-lkl-nonrelease/bin/sgx-lkl-disk create --size=250M --docker=$DOCKER_SQLITE_IMAGE --encrypt --integrity --key-file $SGX_LK_IMAGE