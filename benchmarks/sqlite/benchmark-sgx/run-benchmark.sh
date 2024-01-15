#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

SGX_LK_IMAGE=$ROOT_DIR/sgx-lkl-sqlite.img
SGX_LK_IMAGE_DIRTY=$SGX_LK_IMAGE.dirty
SGX_LK_IMAGE_KEY=$SGX_LK_IMAGE.key

rm -f $SGX_LK_IMAGE_DIRTY
cp $SGX_LK_IMAGE $SGX_LK_IMAGE_DIRTY

SGXLKL_HD_KEY=${SGX_LK_IMAGE_KEY} /opt/sgx-lkl-nonrelease/bin/sgx-lkl-run-oe \
    --enclave-config $ROOT_DIR/enclave_config.json --hw-debug ${SGX_LK_IMAGE_DIRTY} app "$@" 2>&1 \
    | grep -v 'reboot: Restarting system' \
    | grep -v 'WARN: WRFSBASE instruction raises SIGILL' \
    | grep -v 'wg0 has public key'