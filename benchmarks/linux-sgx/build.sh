#!/bin/bash
ROOT_DIR=$(dirname $(realpath $0))

# Check that a compilation with profiling flasg is requested
PROFILING_FLAG_MAKE=""
if [ $1 = "profiling" ]; then
   PROFILING_FLAG_MAKE="BENCHMARK_PROFILING=1"
   echo "Profiling of the SDK enabled"
fi

echo "[+] Building sgx_tprotected_fs"
cd $ROOT_DIR/sdk/protected_fs/sgx_tprotected_fs/
make clean
make ${PROFILING_FLAG_MAKE}
mv $ROOT_DIR/build/linux/libsgx_tprotected_fs.a $ROOT_DIR/build/linux/libsgx_tprotected_fs_benchmark.a

echo "[+] Building sgx_uprotected_fs"
cd $ROOT_DIR/sdk/protected_fs/sgx_uprotected_fs/
make clean
make ${PROFILING_FLAG_MAKE}
mv $ROOT_DIR/build/linux/libsgx_uprotected_fs.a $ROOT_DIR/build/linux/libsgx_uprotected_fs_benchmark.a