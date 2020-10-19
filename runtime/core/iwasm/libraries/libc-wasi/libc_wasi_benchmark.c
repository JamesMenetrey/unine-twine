#include "libc_wasi_benchmark.h"
#include "libc_wasi_benchmark_utils.h"

#ifdef BENCHMARK_PROFILING

wasi_tracked_time_t currently_wasi_tracked_time = {}; 
wasi_tracked_time_t sum_of_wasi_tracked_time = {};
static int profiling_current_level = 0;


wasi_tracked_time_t* benchmark_get_sum_of_wasi_tracked_time()
{
    return &sum_of_wasi_tracked_time;
}

void benchmark_reset_sum_of_wasi_tracked_time()
{
    memset(&currently_wasi_tracked_time, 0, sizeof(wasi_tracked_time_t));
	memset(&sum_of_wasi_tracked_time, 0, sizeof(wasi_tracked_time_t));
}

void profiling_wasi_set_level(int level)
{
    profiling_current_level = level;
}

int profiling_wasi_get_level()
{
    return profiling_current_level;
}

#endif