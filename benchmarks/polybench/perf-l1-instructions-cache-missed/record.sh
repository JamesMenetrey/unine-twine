#!/bin/bash

SCRIPT_PATH=$(dirname $(realpath $0))

# Try to get the PID of the app to record
DERICHE_PID=$(pgrep deriche)
GRAMSCHMIDT_PID=$(pgrep gramschmidt)
SYMM_PID=$(pgrep symm)
WASM_PID=$(pgrep benchmark-wasm)

if [[ -n "$DERICHE_PID" ]]; then
    PID=$DERICHE_PID
elif [[ -n "$GRAMSCHMIDT_PID" ]]; then
    PID=$GRAMSCHMIDT_PID
elif [[ -n "$SYMM_PID" ]]; then
    PID=$SYMM_PID
elif [[ -n "$WASM_PID" ]]; then
    PID=$WASM_PID
else
    echo "No application to monitor has been started (deriche/gramschmidt/benchmark-wasm supported)."
    exit 1
fi

sudo perf record -e L1-dcache-load-misses -o $SCRIPT_PATH/report.tmp -p $PID