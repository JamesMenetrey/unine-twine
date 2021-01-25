#!/bin/bash

ROOT_DIR=$(dirname $(realpath $0))/../../

LIB_TRUSTED=$(stat --printf="%s" /opt/sgx-lkl-nonrelease/lib/libsgxlkl.so.signed)

echo $((LIB_TRUSTED))