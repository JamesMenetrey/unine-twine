#!/bin/bash
SCRIPT_PATH=$(dirname $(realpath $0))

for size in {5000..175000..5000}
do
    $SCRIPT_PATH/profile_single_value.sh $size
done