#ifndef _LIBC_WASI_BENCHMARK_UTILS_H
#define _LIBC_WASI_BENCHMARK_UTILS_H

#ifdef BENCHMARK_PROFILING

#include <stdio.h>
#include <time.h>

#include "wasmtime_ssp.h"
#include "wasi_common.h"
#include "libc_wasi_benchmark.h"

#define BENCHMARK_START(field, level) \
    if (profiling_wasi_get_level() == level){ \
        start_tracking_time(&currently_wasi_tracked_time.field); \
    }
#define BENCHMARK_STOP(field, level) \
    if (profiling_wasi_get_level() == level){ \
        stop_tracking_time(&currently_wasi_tracked_time.field, &sum_of_wasi_tracked_time.field); \
        sum_of_wasi_tracked_time.count_##field++; \
    }

extern wasi_tracked_time_t currently_wasi_tracked_time;
extern wasi_tracked_time_t sum_of_wasi_tracked_time;

inline void start_tracking_time(clock_t* tracking_field)
{
    if (*tracking_field != 0)
    {
        os_printf("[wasi_benchmark]: error, the tracking time is already started.\n");
        return;
    }
    
    *tracking_field = os_clock();
}

inline void stop_tracking_time(clock_t* tracking_field, clock_t* sum_field)
{
    clock_t current_time = os_clock();

    if (*tracking_field == 0)
    {
        os_printf("[wasi_benchmark]: tracking file writing is not started.\n");
        return;
    }

    *sum_field += current_time - *tracking_field;
    *tracking_field = 0;
}

#else

#define BENCHMARK_START(field, level)
#define BENCHMARK_STOP(field, level)

#endif

#endif /* _LIBC_WASI_BENCHMARK_UTILS_H */