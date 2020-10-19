/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "platform_api_vmcore.h"
#include "platform_api_extension.h"

static os_clock_gettime_t clock_gettime = NULL;
static os_clock_t clock = NULL;

void os_set_clock_gettime(os_clock_gettime_t pf)
{
    clock_gettime = pf;
}

int os_clock_gettime(clockid_t clk_id, struct timespec *ts)
{
    return clock_gettime(clk_id, ts);
}

uint64
os_time_get_boot_microsecond()
{
    struct timespec ts;
    if (os_clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }

    return ((uint64) ts.tv_sec) * 1000 * 1000 + ((uint64)ts.tv_nsec) / 1000;
}


void os_set_clock(os_clock_t pf)
{
    clock = pf;
}

clock_t os_clock()
{
    return clock();
}