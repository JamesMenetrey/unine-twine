#pragma once

#ifndef _BENCHMARK_TPROTECTED_FS_H_
#define _BENCHMARK_TPROTECTED_FS_H_

#include <stdio.h>
#include <time.h>

typedef struct {
    clock_t crypto_static;
    long int count_crypto_static;
    clock_t crypto_read_data_node;
    long int count_crypto_read_data_node;
    clock_t crypto_read_mht_node;
    long int count_crypto_read_mht_node;
    clock_t fopen_auto_key;
    long int count_fopen_auto_key;
    clock_t fwrite;
    long int count_fwrite;
    clock_t fread;
    long int count_fread;
    clock_t ftell;
    long int count_ftell;
    clock_t fseek;
    long int count_fseek;
    clock_t fflush;
    long int count_fflush;
    clock_t feof;
    long int count_feof;
    clock_t fclose;
    long int count_fclose;
    clock_t get_data_node;
    long int count_get_data_node;
    clock_t append_data_node;
    long int count_append_data_node;
    clock_t read_data_node;
    long int count_read_data_node;
    clock_t get_mht_node;
    long int count_get_mht_node;
    clock_t append_mht_node;
    long int count_append_mht_node;
    clock_t read_mht_node;
    long int count_read_mht_node;
    clock_t internal_flush_node;
    long int count_internal_flush_node;
    clock_t memcpy_write;
    long int count_memcpy_write;
    clock_t memcpy_read;
    long int count_memcpy_read;
    clock_t memset_get_data_node;
    long int count_memset_get_data_node;
    clock_t memset_append_data_node;
    long int count_memset_append_data_node;
    clock_t memset_read_data_node;
    long int count_memset_read_data_node;
    clock_t memset_append_mht_node;
    long int count_memset_append_mht_node;
    clock_t memset_read_mht_node;
    long int count_memset_read_mht_node;
    clock_t cache_get_data_node_get;
    long int count_cache_get_data_node_get;
    clock_t cache_get_data_node_size;
    long int count_cache_get_data_node_size;
    clock_t cache_get_data_node_remove_last;
    long int count_cache_get_data_node_remove_last;
    clock_t cache_get_data_node_get_last;
    long int count_cache_get_data_node_get_last;
    clock_t cache_append_data_node_add;
    long int count_cache_append_data_node_add;
    clock_t cache_read_data_node_get;
    long int count_cache_read_data_node_get;
    clock_t cache_read_data_node_add;
    long int count_cache_read_data_node_add;
    clock_t cache_read_mht_node_find;
    long int count_cache_read_mht_node_find;
    clock_t cache_append_mht_node_find;
    long int count_cache_append_mht_node_find;
    clock_t cache_append_mht_node_add;
    long int count_cache_append_mht_node_add;
    clock_t cache_read_mht_node_add;
    long int count_cache_read_mht_node_add;
    clock_t fread_node_read_data_node;
    long int count_fread_node_read_data_node;
    clock_t fread_node_read_mht_node;
    long int count_fread_node_read_mht_node;
} trusted_tracked_time_t;

#define PROFILING_TRUSTED_LEVEL_BASE 1000
#define PROFILING_TRUSTED_LEVEL_ATOMIC PROFILING_TRUSTED_LEVEL_BASE + 1
#define PROFILING_TRUSTED_LEVEL_FILE_API PROFILING_TRUSTED_LEVEL_BASE + 2
#define PROFILING_TRUSTED_LEVEL_GET_DATA_NODE PROFILING_TRUSTED_LEVEL_BASE + 3
#define PROFILING_TRUSTED_LEVEL_APPEND_DATA_NODE PROFILING_TRUSTED_LEVEL_BASE + 4
#define PROFILING_TRUSTED_LEVEL_READ_DATA_NODE PROFILING_TRUSTED_LEVEL_BASE + 5
#define PROFILING_TRUSTED_LEVEL_GET_MHT_NODE PROFILING_TRUSTED_LEVEL_BASE + 6
#define PROFILING_TRUSTED_LEVEL_READ_MHT_NODE PROFILING_TRUSTED_LEVEL_BASE + 7
#define PROFILING_TRUSTED_LEVEL_APPEND_MHT_NODE PROFILING_TRUSTED_LEVEL_BASE + 8
#define PROFILING_TRUSTED_LEVEL_INTERNAL_FLUSH PROFILING_TRUSTED_LEVEL_BASE + 9

#ifdef __cplusplus
extern "C" {
#endif

void* benchmark_get_sum_of_tracked_time();
void benchmark_reset_sum_of_trusted_tracked_time();
void profiling_trusted_set_level(int level);
int profiling_trusted_get_level();

#ifdef __cplusplus
}
#endif
#endif