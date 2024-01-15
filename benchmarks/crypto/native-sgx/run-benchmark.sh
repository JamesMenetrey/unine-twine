#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

set -e

BENCHMARK_DIR=native-sgx
SGX_LK_IMAGE=$SCRIPT_DIR/sgx-lkl-wolfssl-crypto.img
SGX_LK_IMAGE_DIRTY=$SGX_LK_IMAGE.dirty
SGX_LK_IMAGE_KEY=$SGX_LK_IMAGE.key

rm -f $SGX_LK_IMAGE_DIRTY
cp $SGX_LK_IMAGE $SGX_LK_IMAGE_DIRTY

SGXLKL_HD_KEY=${SGX_LK_IMAGE_KEY} /opt/sgx-lkl-nonrelease/bin/sgx-lkl-run-oe \
    --enclave-config $SCRIPT_DIR/enclave_config.json --hw-debug ${SGX_LK_IMAGE_DIRTY} wasm_example -b 2>&1 \
    | grep -v 'reboot: Restarting system' \
    | grep -v 'WARN: WRFSBASE instruction raises SIGILL' \
    | grep -v 'wg0 has public key'