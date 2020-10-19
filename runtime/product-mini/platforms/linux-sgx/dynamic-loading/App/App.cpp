/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Enclave_u.h"
#include "sgx_urts.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define WASM_FILENAME "../sqlite-test.wasm"
#define TOKEN_FILENAME   "enclave.token"
#define ENCLAVE_FILENAME "enclave.signed.so"
#define MAX_PATH FILENAME_MAX

sgx_enclave_id_t g_eid = 0;

void ocall_print(const char* str)
{
    printf("OCALL_print: %s", str);
}

clock_t ocall_clock()
{
    return clock();
}

int ocall_clock_gettime(clockid_t clk_id, struct timespec *ts)
{
    printf("OCALL_clock_gettime called.\n");
    return clock_gettime(clk_id, ts);
}

int ocall_stat(const char* path, void *buf, unsigned int buf_len)
{
    return stat(path, (struct stat *)buf);
}

int ocall_fstat(int fd, void *buf, unsigned int buf_len)
{
    return fstat(fd, (struct stat *)buf);
}

int ocall_fstatat(int dirfd, const char *pathname, void *buf,
                  unsigned int buf_len, int flags)
{
    return fstatat(dirfd, pathname, (struct stat *)buf, flags);
}

int ocall_get_errno()
{
    return errno;
}

int ocall_fcntl(int fd, int cmd)
{
  return fcntl(fd, cmd);
}

static void print_error_message(sgx_status_t ret)
{
    printf("SGX error code: %d\n", ret);
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
static int enclave_init(sgx_enclave_id_t *p_eid)

{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;

    /* Step 1: try to retrieve the launch token saved by last transaction 
     *         if there is no token, then create a new one.
     */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;

    if (home_dir != NULL &&
        (strlen(home_dir) + strlen("/") + sizeof(TOKEN_FILENAME) + 1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME) + 1);
    }
    else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }

    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, p_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        if (fp != NULL)
            fclose(fp);
        return -1;
    }

    /* Step 3: save the launch token if it is updated */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL)
        return 0;

    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);

    fclose(fp);
    return 0;
}

int main(int argc, char const *argv[])
{
    if (enclave_init(&g_eid) < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return 1;
    }

    FILE *fp = fopen(WASM_FILENAME, "rb");
    if (fp == NULL) {
        std::cout << "Fail to open the file \"" << WASM_FILENAME << "\"." << std::endl;
        return 1;
    }
    
    fseek(fp, 0L, SEEK_END);
    long int wasm_buffer_len = ftell(fp);
    rewind(fp);

    uint8_t* wasm_buffer = new uint8_t[wasm_buffer_len];

    size_t num_read = fread(wasm_buffer, sizeof(uint8_t), wasm_buffer_len, fp);

    if (num_read != wasm_buffer_len) {
        std::cout << "Fail to read the file \"" << WASM_FILENAME << "\"." << std::endl;
        return 1;
    }

    printf("== ECALL start == \n");
    ecall_load_wasm(g_eid, wasm_buffer, wasm_buffer_len);
    printf("== ECALL end == \n");

    fclose(fp);

    return 0;
}

