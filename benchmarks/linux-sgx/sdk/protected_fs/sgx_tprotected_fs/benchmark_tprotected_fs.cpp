#include "benchmark_tprotected_fs.h"

#include <string.h>

trusted_tracked_time_t currently_tracked_time = {};
trusted_tracked_time_t sum_of_tracked_time = {};
static int profiling_current_level = 0;

void* benchmark_get_sum_of_tracked_time()
{
    return &sum_of_tracked_time;
}

void benchmark_reset_sum_of_trusted_tracked_time()
{
	memset(&currently_tracked_time, 0, sizeof(trusted_tracked_time_t));
	memset(&sum_of_tracked_time, 0, sizeof(trusted_tracked_time_t));
}

void profiling_trusted_set_level(int level)
{
	profiling_current_level = level;
}

int profiling_trusted_get_level()
{
	return profiling_current_level;
}