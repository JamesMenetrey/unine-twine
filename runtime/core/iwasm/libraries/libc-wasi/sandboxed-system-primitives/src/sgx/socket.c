#include "wasi_common.h"

// Receive a message from a socket.
// Note: This is similar to recv in POSIX, though it also supports reading the data into multiple buffers in the manner of readv.
__wasi_errno_t wasmtime_ssp_sock_recv(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t sock,
    const __wasi_iovec_t *ri_data,
    size_t ri_data_len,
    __wasi_riflags_t ri_flags,
    size_t *ro_datalen,
    __wasi_roflags_t *ro_flags
) {
    os_printf("Not implemented WASI function called: 'sock_recv'.\n");
    return __WASI_ECANCELED;
}

// Send a message on a socket.
// Note: This is similar to send in POSIX, though it also supports writing the data from multiple buffers in the manner of writev.
__wasi_errno_t wasmtime_ssp_sock_send(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t sock,
    const __wasi_ciovec_t *si_data,
    size_t si_data_len,
    __wasi_siflags_t si_flags,
    size_t *so_datalen
) NO_LOCK_ANALYSIS {
    os_printf("Not implemented WASI function called: 'sock_send'.\n");
    return __WASI_ECANCELED;
}

// Shut down socket send and receive channels. Note: This is similar to shutdown in POSIX.
__wasi_errno_t wasmtime_ssp_sock_shutdown(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t sock,
    __wasi_sdflags_t how
) {
    os_printf("Not implemented WASI function called: 'sock_shutdown'.\n");
    return __WASI_ECANCELED;
}