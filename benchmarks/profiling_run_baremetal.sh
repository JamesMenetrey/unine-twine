#!/bin/bash
SCRIPT_PATH=$(dirname $(realpath $0))

set -e

if [ $# -eq 0 ] ; then
    $SCRIPT_PATH/sqlite/profile_all_values.sh
else
    $SCRIPT_PATH/sqlite/profile_single_value.sh $1
fi
