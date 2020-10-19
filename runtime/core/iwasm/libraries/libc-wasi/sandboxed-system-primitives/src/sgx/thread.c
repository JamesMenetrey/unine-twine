#include "wasi_common.h"

__wasi_errno_t wasmtime_ssp_sched_yield(void) {
    os_printf("Not implemented WASI function called: 'sched_yield'.\n");
    return __WASI_ECANCELED;
}