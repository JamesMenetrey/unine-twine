#include "wasi_common.h"
#include "common/time.h"
#include "libc_wasi_benchmark_utils.h"

// The last monotonic values retrieved, used to prevent the untrusted platform to give past values.
static __wasi_timestamp_t last_clock_monotonic = 0;
static __wasi_timestamp_t last_clock_process_cputime = 0;
static __wasi_timestamp_t last_clock_thread_cputime = 0;

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

    ENSURE_UNTRUSTED_CALL_ENABLED("clock_time_get");

    // Convert the WASI clock identifier into the POSIX identifier
    clockid_t nclock_id;
    if (!convert_clockid(clock_id, &nclock_id)) return __WASI_EINVAL;
    
    // Retrieve the time from the untrusted part
    struct timespec ts;
    if (os_clock_gettime(nclock_id, &ts) < 0) return __WASI_ENOSYS;
    __wasi_timestamp_t untrusted_time = convert_timespec(&ts);

    // Sanity check: in case of monotonic time, is it greater than the previous one ?
    switch(clock_id)
    {
        case __WASI_CLOCK_MONOTONIC:
            if (last_clock_monotonic >= untrusted_time) return __WASI_ENOSYS;
            last_clock_monotonic = untrusted_time;
            break;

        case __WASI_CLOCK_PROCESS_CPUTIME_ID:
            if (last_clock_process_cputime >= untrusted_time) return __WASI_ENOSYS;
            last_clock_process_cputime = untrusted_time;
            break;

        case __WASI_CLOCK_THREAD_CPUTIME_ID:
            if (last_clock_thread_cputime >= untrusted_time) return __WASI_ENOSYS;
            last_clock_thread_cputime = untrusted_time;
            break;
    }
    
    // Returns the time by argument pointer
    *time = untrusted_time;

    BENCHMARK_STOP(clock_time_get, PROFILING_WASI_LEVEL_WASI_API);
    return __WASI_ESUCCESS;
}