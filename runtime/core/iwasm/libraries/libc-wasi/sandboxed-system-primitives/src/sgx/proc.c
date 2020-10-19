#include "wasi_common.h"

// Terminate the process normally.
// An exit code of 0 indicates successful termination of the program. The meanings of other values is dependent on the environment.
void wasmtime_ssp_proc_exit(
    __wasi_exitcode_t rval
) {
    os_printf("Not implemented WASI function called: 'proc_exit'.\n");
}

// Send a signal to the process of the calling thread. Note: This is similar to raise in POSIX.
__wasi_errno_t wasmtime_ssp_proc_raise(
    __wasi_signal_t sig
) {
    os_printf("Not implemented WASI function called: 'proc_raise'.\n");
    return __WASI_ECANCELED;
}