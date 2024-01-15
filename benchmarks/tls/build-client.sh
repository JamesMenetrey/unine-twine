#!/bin/bash

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
set -e

cd client
make

cp -r $SCRIPT_DIR/../wolfssl/certs build/
tar -czf client.tar.gz -C build .
echo "> The client and certificates have been compressed in client.tar.gz"