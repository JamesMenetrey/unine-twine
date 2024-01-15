#!/bin/bash

set -e

# Instructions on https://github.com/lsds/sgx-lkl

echo "deb [trusted=yes] https://clcpackages.blob.core.windows.net/apt-dev/1fa5fb889b8efa6ea07354c3b54903f7 ./" | sudo tee /etc/apt/sources.list.d/azure-clc.list

sudo apt update
sudo apt install sgx-lkl-nonrelease=0.2.0-dev.20210412.1-b6e838e
/opt/sgx-lkl-nonrelease/bin/sgx-lkl-setup

echo "Add '/opt/sgx-lkl-nonrelease/bin' to your PATH. Typically: PATH=\"\$PATH:/opt/sgx-lkl-nonrelease/bin\""
