#!/bin/bash

SCRIPT_DIR=$(dirname $(realpath $0))

$SCRIPT_DIR/common_build_docker.sh
$SCRIPT_DIR/sqlite/cost-factors/compilations/sgx/build.sh