#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

set -e

BENCHMARK_DIR=native-sgx
SGX_LK_IMAGE=$SCRIPT_DIR/sgx-lkl-wolfssl-tls.img
SGX_LK_IMAGE_DIRTY=$SGX_LK_IMAGE.dirty
SGX_LK_IMAGE_KEY=$SGX_LK_IMAGE.key

rm -f $SGX_LK_IMAGE_DIRTY
cp $SGX_LK_IMAGE $SGX_LK_IMAGE_DIRTY

# If the following throws the error "FAIL: virtio_net_fd_net_tx: write failed: fd=4 ret=-1 errno=5 Input/output error", you need to first run
# the script /opt/sgx-lkl-nonrelease/bin/sgx-lkl-setup.

#Optional debug flags: SGXLKL_VERBOSE=1 SGXLKL_KERNEL_VERBOSE=1
SGXLKL_HD_KEY=${SGX_LK_IMAGE_KEY} SGXLKL_TAP=sgxlkl_tap0 /opt/sgx-lkl-nonrelease/bin/sgx-lkl-run-oe \
    --enclave-config $SCRIPT_DIR/enclave_config.json --hw-debug ${SGX_LK_IMAGE_DIRTY} server-tls-poll-perf -p 11112 $@ 2>&1 \
    | grep -v 'reboot: Restarting system' \
    | grep -v 'WARN: WRFSBASE instruction raises SIGILL' \
    | grep -v 'wg0 has public key'