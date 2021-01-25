#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

SGX_LK_IMAGE=$ROOT_DIR/sgx-lkl-sqlite.img
SGX_LK_IMAGE_DIRTY=$SGX_LK_IMAGE.dirty
SGX_LK_IMAGE_KEY=$SGX_LK_IMAGE.key

rm -f $SGX_LK_IMAGE_DIRTY
cp $SGX_LK_IMAGE $SGX_LK_IMAGE_DIRTY

SGXLKL_HD_KEY=${SGX_LK_IMAGE_KEY} python3 $ROOT_DIR/../time_estimation.py /opt/sgx-lkl-nonrelease/bin/sgx-lkl-run-oe --hw-debug $ROOT_DIR/sgx-lkl-sqlite.img.dirty app "$@"