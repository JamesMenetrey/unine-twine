#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))/../../

EXE=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/bin/sgx-lkl-run-oe)
LIB_EXTERNAL_LIBRT=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/external/librt.so.1)
LIB_EXTERNAL_LIBDL=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/external/libdl.so.2)
LIB_EXTERNAL_LIBCRYPTO=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/external/libcrypto.so.1.1)
LIB_EXTERNAL_LIBPTHREAD=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/external/libpthread.so.0)
LIB_EXTERNAL_LIBC=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/external/libc.so.6)
LIB_UNTRUSTED=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/libsgxlkl-user.so)

echo $((EXE + LIB_EXTERNAL_LIBRT + LIB_EXTERNAL_LIBDL + LIB_EXTERNAL_LIBCRYPTO + LIB_EXTERNAL_LIBPTHREAD + LIB_EXTERNAL_LIBC + LIB_UNTRUSTED))