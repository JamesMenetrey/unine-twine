#include "wasi_common.h"
#include "common/time.h"
#include "libc_wasi_benchmark_utils.h"

// Return the resolution of a clock. Implementations are required to provide a non-zero value for supported clocks.
// For unsupported clocks, return errno::inval. Note: This is similar to clock_getres in POSIX.
__wasi_errno_t wasmtime_ssp_clock_res_get(
    __wasi_clockid_t clock_id,
    __wasi_timestamp_t *resolution
) {
    os_printf("Not implemented WASI function called: 'clock_res_get'\n");
    return __WASI_ECANCELED;
}

// Return the time value of a clock. Note: This is similar to clock_gettime in POSIX.
__wasi_errno_t wasmtime_ssp_clock_time_get(
    __wasi_clockid_t clock_id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *time
) {
    BENCHMARK_START(clock_time_get, PROFILING_WASI_LEVEL_WASI_API);

    clockid_t nclock_id;
    if (!convert_clockid(clock_id, &nclock_id)) return __WASI_EINVAL;
    
    struct timespec ts;
    if (os_clock_gettime(nclock_id, &ts) < 0) return __WASI_ENOSYS;
    
    *time = convert_timespec(&ts);

    BENCHMARK_STOP(clock_time_get, PROFILING_WASI_LEVEL_WASI_API);
    return __WASI_ESUCCESS;
}