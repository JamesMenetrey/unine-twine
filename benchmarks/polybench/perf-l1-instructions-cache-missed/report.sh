#!/bin/bash

SCRIPT_PATH=$(dirname $(realpath $0))

sudo perf report -i $SCRIPT_PATH/report.tmp