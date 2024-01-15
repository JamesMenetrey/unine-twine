#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))

SGX_LK_IMAGE=sgxlkl-nodejs.img

SGXLKL_HEAP=2048MB /opt/sgx-lkl-nonrelease/bin/sgx-lkl-run-oe --hw-debug ${SGX_LK_IMAGE} /usr/bin/node /app/"$@" 2>&1 \
    | grep -v 'reboot: Restarting system' \
    | grep -v 'WARN: WRFSBASE instruction raises SIGILL' \
    | grep -v 'wg0 has public key'