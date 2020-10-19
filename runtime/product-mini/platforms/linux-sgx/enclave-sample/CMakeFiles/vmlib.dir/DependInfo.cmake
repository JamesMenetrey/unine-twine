# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  "C"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/arch/invokeNative_em64.s" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/arch/invokeNative_em64.s.o"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "BH_FREE=wasm_runtime_free"
  "BH_MALLOC=wasm_runtime_malloc"
  "BH_PLATFORM_LINUX_SGX"
  "BUILD_TARGET_X86_64"
  "WASM_ENABLE_BULK_MEMORY=0"
  "WASM_ENABLE_FAST_INTERP=0"
  "WASM_ENABLE_INTERP=1"
  "WASM_ENABLE_LIBC_BUILTIN=1"
  "WASM_ENABLE_LIBC_WASI=1"
  "WASM_ENABLE_MINI_LOADER=0"
  "WASM_ENABLE_MULTI_MODULE=0"
  "WASM_ENABLE_SGX=1"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "../../../../core/shared/include"
  "../../../../core/iwasm/include"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/../include"
  "/opt/intel/sgxsdk/include"
  "/opt/intel/sgxsdk/include/tlibc"
  "/opt/intel/sgxsdk/include/libcxx"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-builtin"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/include"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/sgx"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter"
  )
set(CMAKE_DEPENDS_CHECK_C
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_exec_env.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_exec_env.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_memory.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_memory.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_native.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_native.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_runtime_common.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common/wasm_runtime_common.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter/wasm_interp_classic.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter/wasm_interp_classic.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter/wasm_loader.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter/wasm_loader.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter/wasm_runtime.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter/wasm_runtime.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-builtin/libc_builtin_wrapper.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-builtin/libc_builtin_wrapper.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/libc_wasi_wrapper.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/libc_wasi_wrapper.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/sgx/arguments.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/sgx/arguments.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/sgx/wasi_sgx.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/sgx/wasi_sgx.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/ems/ems_alloc.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/ems/ems_alloc.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/ems/ems_hmu.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/ems/ems_hmu.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/ems/ems_kfc.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/ems/ems_kfc.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/mem_alloc.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc/mem_alloc.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/sgx_platform.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/sgx_platform.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/sgx_thread.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/sgx_thread.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/sgx_time.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/sgx_time.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_assert.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_assert.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_common.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_common.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_hashmap.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_hashmap.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_list.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_list.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_log.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_log.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_queue.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_queue.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_vector.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/bh_vector.c.o"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/runtime_timer.c" "/home/james/dev/gitlab/wasm-micro-runtime/product-mini/platforms/linux-sgx/enclave-sample/CMakeFiles/vmlib.dir/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils/runtime_timer.c.o"
  )
set(CMAKE_C_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_C
  "BH_FREE=wasm_runtime_free"
  "BH_MALLOC=wasm_runtime_malloc"
  "BH_PLATFORM_LINUX_SGX"
  "BUILD_TARGET_X86_64"
  "WASM_ENABLE_BULK_MEMORY=0"
  "WASM_ENABLE_FAST_INTERP=0"
  "WASM_ENABLE_INTERP=1"
  "WASM_ENABLE_LIBC_BUILTIN=1"
  "WASM_ENABLE_LIBC_WASI=1"
  "WASM_ENABLE_MINI_LOADER=0"
  "WASM_ENABLE_MULTI_MODULE=0"
  "WASM_ENABLE_SGX=1"
  )

# The include file search paths:
set(CMAKE_C_TARGET_INCLUDE_PATH
  "../../../../core/shared/include"
  "../../../../core/iwasm/include"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/platform/linux-sgx/../include"
  "/opt/intel/sgxsdk/include"
  "/opt/intel/sgxsdk/include/tlibc"
  "/opt/intel/sgxsdk/include/libcxx"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/mem-alloc"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/shared/utils"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-builtin"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/include"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/sgx"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/common"
  "/home/james/dev/gitlab/wasm-micro-runtime/core/iwasm/interpreter"
  )

# Targets to which this target links.
set(CMAKE_TARGET_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
