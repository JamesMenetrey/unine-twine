#ifndef _LIBC_WASI_BENCHMARK_H
#define _LIBC_WASI_BENCHMARK_H

#include <time.h>

#define PROFILING_WASI_LEVEL_BASE 3000
#define PROFILING_WASI_LEVEL_WASI_API PROFILING_WASI_LEVEL_BASE + 1

typedef struct {
    clock_t fd_close;
    long int count_fd_close;
    clock_t fd_fdstat_get;
    long int count_fd_fdstat_get;
    clock_t fd_filestat_get;
    long int count_fd_filestat_get;
    clock_t fd_prestat_get;
    long int count_fd_prestat_get;
    clock_t fd_prestat_dir_name;
    long int count_fd_prestat_dir_name;
    clock_t fd_read;
    long int count_fd_read;
    clock_t fd_seek;
    long int count_fd_seek;
    clock_t fd_sync;
    long int count_fd_sync;
    clock_t fd_tell;
    long int count_fd_tell;
    clock_t fd_write;
    long int count_fd_write;
    clock_t path_filestat_get;
    long int count_path_filestat_get;
    clock_t path_open;
    long int count_path_open;
    clock_t clock_time_get;
    long int count_clock_time_get;
} wasi_tracked_time_t;


#ifdef __cplusplus
extern "C" {
#endif

wasi_tracked_time_t* benchmark_get_sum_of_wasi_tracked_time();
void benchmark_reset_sum_of_wasi_tracked_time();
void profiling_wasi_set_level(int level);
int profiling_wasi_get_level();

#ifdef __cplusplus
}
#endif

#endif /* _LIBC_WASI_BENCHMARK_H */