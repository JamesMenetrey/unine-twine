#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

cd $SCRIPT_DIR
docker build -t unine_sgx_lkl_builder .