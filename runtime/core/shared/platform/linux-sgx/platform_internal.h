/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _PLATFORM_INTERNAL_H
#define _PLATFORM_INTERNAL_H

#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <sgx_thread.h>

#include "sgx/wasi_datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BH_PLATFORM_LINUX_SGX
#define BH_PLATFORM_LINUX_SGX
#endif

#define _STACK_SIZE_ADJUSTMENT (32 * 1024)

/* Stack size of applet threads's native part.  */
#define BH_APPLET_PRESERVED_STACK_SIZE      (8 * 1024 + _STACK_SIZE_ADJUSTMENT)

/* Default thread priority */
#define BH_THREAD_DEFAULT_PRIORITY 0

typedef sgx_thread_t korp_thread;
typedef sgx_thread_t korp_tid;
typedef sgx_thread_mutex_t korp_mutex;
typedef sgx_thread_cond_t korp_cond;
typedef __clock_t clock_t;

typedef void (*os_print_function_t)(const char* message);
void os_set_print_function(os_print_function_t pf);

typedef int (*os_clock_gettime_t)(clockid_t clk_id, struct timespec *ts);
void os_set_clock_gettime(os_clock_gettime_t pf);
int os_clock_gettime(clockid_t clk_id, struct timespec *ts);

typedef clock_t (*os_clock_t)();
void os_set_clock(os_clock_t pf);
clock_t os_clock();

int check_if_file_exists(const char* filename);
int get_errno(void);
int stat(const char *path, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);
int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);
int fcntl(int fd, int cmd);

#ifdef __cplusplus
}
#endif

#endif /* end of _PLATFORM_INTERNAL_H */

