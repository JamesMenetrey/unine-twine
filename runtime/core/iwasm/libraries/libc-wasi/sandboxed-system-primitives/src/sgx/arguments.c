#include "wasi_common.h"

// Read command-line argument data. The size of the array should match that returned by args_sizes_get.
__wasi_errno_t wasmtime_ssp_args_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
  struct argv_environ_values *argv_environ,
#endif
  char **argv,
  char *argv_buf
) {
  for (size_t i = 0; i < argv_environ->argc; ++i) {
    argv[i] = argv_buf + (argv_environ->argv[i] - argv_environ->argv_buf);
  }
  argv[argv_environ->argc] = NULL;
  bh_memcpy_s(argv_buf, (uint32)argv_environ->argv_buf_size,
              argv_environ->argv_buf, (uint32)argv_environ->argv_buf_size);
  return __WASI_ESUCCESS;
}

// Return command-line argument data sizes.
__wasi_errno_t wasmtime_ssp_args_sizes_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
  struct argv_environ_values *argv_environ,
#endif
  size_t *argc,
  size_t *argv_buf_size
) {
  *argc = argv_environ->argc;
  *argv_buf_size = argv_environ->argv_buf_size;
  return __WASI_ESUCCESS;
}