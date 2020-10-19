/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "platform_api_vmcore.h"
#include "platform_api_extension.h"
#include "sgx_error.h"
#include "bh_common.h"

int ocall_get_errno(int *p_ret);
int ocall_fcntl(int *p_ret, int fd, int cmd);
int ocall_stat(int *p_ret, const char *path, void *buf, unsigned int buf_len);
int ocall_fstat(int *p_ret, int fd, void *buf, unsigned int buf_len);
int ocall_fstatat(int *p_ret, int dirfd, const char *pathname, void *buf,
                  unsigned int buf_len, int flags);

int get_errno(void)
{
    int ret;

    if (ocall_get_errno(&ret) != SGX_SUCCESS) {
        return -1;
    }
    return ret;
}

int stat(const char *path, struct stat *statbuf)
{
    int ret;

    if (statbuf == NULL)
        return -1;
        
    if (ocall_stat(&ret, path, (void *)statbuf,
                    sizeof(struct stat)) != SGX_SUCCESS) {
        return -1;
    }

    if (ret == -1)
        errno = get_errno();
    return ret;
}

int fstat(int fd, struct stat *statbuf)
{
    int ret;

    if (statbuf == NULL)
        return -1;
        
    if (ocall_fstat(&ret, fd, (void *)statbuf,
                    sizeof(struct stat)) != SGX_SUCCESS) {
        return -1;
    }

    if (ret == -1)
        errno = get_errno();
    return ret;
}

int fstatat(int dirfd, const char *pathname, struct stat *statbuf,
            int flags)
{
    int ret;

    if (statbuf == NULL)
        return -1;

    if (ocall_fstatat(&ret, dirfd, pathname, (void *)statbuf,
                      sizeof(struct stat), flags) != SGX_SUCCESS) {
        return -1;
    }

    if (ret == -1)
        errno = get_errno();
    return ret;
}

int fcntl(int fd, int cmd)
{
    int ret;

    switch (cmd) {
        case F_GETFD:
        case F_GETFL:
            if (ocall_fcntl(&ret, fd, cmd) != SGX_SUCCESS) {
                return -1;
            }
            break;

        case F_DUPFD:
        case F_SETFD:
        case F_SETFL:
        default:
            os_printf("fcntl failed: unknown cmd %d.\n", cmd);
            return -1;
    }

    if (ret == -1)
        errno = get_errno();
    return ret;
}