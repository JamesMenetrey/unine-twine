#pragma once

#ifndef _BENCHMARK_UTILS_H_
#define _BENCHMARK_UTILS_H_

#include "sgx_tcrypto.h"
#include "sgx_tprotected_fs_t.h"
#include "protected_fs_file.h"


#ifdef BENCHMARK_PROFILING

#include <stdio.h>
#include <time.h>

#include "benchmark_tprotected_fs.h"

#define BENCHMARK_START(field, level) \
    if (profiling_trusted_get_level() == level){ \
        start_tracking_time(&currently_tracked_time.field); \
    }
#define BENCHMARK_STOP(field, level) \
    if (profiling_trusted_get_level() == level){ \
        stop_tracking_time(&currently_tracked_time.field, &sum_of_tracked_time.field); \
        sum_of_tracked_time.count_##field++; \
    }

extern trusted_tracked_time_t currently_tracked_time;
extern trusted_tracked_time_t sum_of_tracked_time;

inline void start_tracking_time(clock_t* tracking_field)
{
    if (*tracking_field != 0)
    {
        u_sgxprotectedfs_print_stderr("[benchmark_tprotected_fs]: error, the tracking time is already started.\n");
        return;
    }

    u_sgxprotectedfs_clock(tracking_field);
}

inline void stop_tracking_time(clock_t* tracking_field, clock_t* sum_field)
{
    clock_t current_time;
    u_sgxprotectedfs_clock(&current_time);

    if (*tracking_field == 0)
    {
        u_sgxprotectedfs_print_stderr("[benchmark_tprotected_fs]: tracking file writing is not started.\n");
        return;
    }

    *sum_field += current_time - *tracking_field;
    *tracking_field = 0;
}

#else

#define BENCHMARK_START(field, level)
#define BENCHMARK_STOP(field, level)

#endif

inline sgx_status_t SGXAPI benchmark_sgx_rijndael128GCM_encrypt(const sgx_aes_gcm_128bit_key_t *p_key,
                                                const uint8_t *p_src,
                                                uint32_t src_len,
                                                uint8_t *p_dst,
                                                const uint8_t *p_iv,
                                                uint32_t iv_len,
                                                const uint8_t *p_aad,
                                                uint32_t aad_len,
                                                sgx_aes_gcm_128bit_tag_t *p_out_mac)
{
    BENCHMARK_START(crypto_static, PROFILING_TRUSTED_LEVEL_ATOMIC);
    sgx_status_t r = sgx_rijndael128GCM_encrypt(p_key, p_src, src_len, p_dst, p_iv, iv_len, p_aad, aad_len, p_out_mac);
    BENCHMARK_STOP(crypto_static, PROFILING_TRUSTED_LEVEL_ATOMIC);
    return r;
}

inline sgx_status_t SGXAPI benchmark_sgx_rijndael128GCM_decrypt(const sgx_aes_gcm_128bit_key_t *p_key,
                                                const uint8_t *p_src,
                                                uint32_t src_len,
                                                uint8_t *p_dst,
                                                const uint8_t *p_iv,
                                                uint32_t iv_len,
                                                const uint8_t *p_aad,
                                                uint32_t aad_len,
                                                const sgx_aes_gcm_128bit_tag_t *p_in_mac)
{
    BENCHMARK_START(crypto_static, PROFILING_TRUSTED_LEVEL_ATOMIC);
    sgx_status_t r = sgx_rijndael128GCM_decrypt(p_key, p_src, src_len, p_dst, p_iv, iv_len, p_aad, aad_len, p_in_mac);
    BENCHMARK_STOP(crypto_static, PROFILING_TRUSTED_LEVEL_ATOMIC);
    return r;
}

#endif