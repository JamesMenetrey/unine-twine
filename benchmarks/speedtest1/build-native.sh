#!/bin/bash

mkdir -p build/native
cd build/native
cmake ../../benchmark -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
make -j`nproc`