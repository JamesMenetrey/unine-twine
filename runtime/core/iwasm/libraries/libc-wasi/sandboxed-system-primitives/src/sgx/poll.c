#include "wasi_common.h"

// Concurrently poll for the occurrence of a set of events.
__wasi_errno_t wasmtime_ssp_poll_oneoff(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    const __wasi_subscription_t *in,
    __wasi_event_t *out,
    size_t nsubscriptions,
    size_t *nevents
) NO_LOCK_ANALYSIS {
    os_printf("Not implemented WASI function called: 'poll_oneoff'.\n");
    return __WASI_ECANCELED;
}