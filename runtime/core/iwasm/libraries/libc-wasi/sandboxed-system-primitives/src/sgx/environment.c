#include "wasi_common.h"

// Read environment variable data. The sizes of the buffers should match that returned by environ_sizes_get.
__wasi_errno_t wasmtime_ssp_environ_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
  struct argv_environ_values *argv_environ,
#endif
  char **environ,
  char *environ_buf
) {
    os_printf("Not implemented WASI function called: 'environ_get'.\n");
    return __WASI_ECANCELED;
}

// Return environment variable data sizes.
__wasi_errno_t wasmtime_ssp_environ_sizes_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
  struct argv_environ_values *argv_environ,
#endif
  size_t *environ_count,
  size_t *environ_buf_size
) {
    os_printf("Not implemented WASI function called: 'environ_sizes_get'.\n");
    return __WASI_ECANCELED;
}