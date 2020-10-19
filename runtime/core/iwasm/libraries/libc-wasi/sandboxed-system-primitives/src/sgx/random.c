#include "wasi_common.h"

// Write high-quality random data into a buffer.
// This function blocks when the implementation is unable to immediately provide sufficient high-quality random data.
// This function may execute slowly, so when large mounts of random data are required, it's advisable to use this function 
// to seed a pseudo-random number generator, rather than to provide the random data directly.
__wasi_errno_t wasmtime_ssp_random_get(
    void *buf,
    size_t nbyte
) {
    os_printf("Not implemented WASI function called: 'random_get'.\n");
    return __WASI_ECANCELED;
}