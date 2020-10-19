/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Enclave_t.h"
#include "wasm_export.h"
#include "benchmark_types.h"
#include "benchmark_tprotected_fs.h"

#define GLOBAL_HEAP_SIZE (240 * 1024 * 1024)
#define HEAP_SIZE (1024 * 1024 * 4)
#define STACK_SIZE (1024 * 1024 * 5)

#define DISPLAY_TIME(operation_type) \
    snprintf(output_buff, sizeof(output_buff), "%s,%d,%d,%d,%ld\n", \
        operation_type, \
        database_type, \
        number_of_write, \
        number_of_read, \
        (end - start)); \
    ocall_print(output_buff);

typedef long int clock_t;

typedef struct {
    clock_t exclusive_file_open;
    long int count_exclusive_file_open;
    clock_t check_if_file_exists;
    long int count_check_if_file_exists;
    clock_t fread_node;
    long int count_fread_node;
    clock_t fwrite_node;
    long int count_fwrite_node;
    clock_t fclose;
    long int count_fclose;
    clock_t fflush;
    long int count_fflush;
    clock_t remove;
    long int count_remove;
    clock_t recovery_file_open;
    long int count_recovery_file_open;
    clock_t fwrite_recovery_node;
    long int count_fwrite_recovery_node;
    clock_t do_file_recovery;
    long int count_do_file_recovery;
} untrusted_tracking_time_t;

static wasm_module_inst_t module_inst = NULL;
static wasm_exec_env_t exec_env = NULL;

#define LOOKUP_WASM_FUNC(func_name) \
    if(!(func_name = wasm_runtime_lookup_function(module_inst, #func_name, NULL))){ \
        ocall_print("The "#func_name" wasm function is not found.\n"); \
        return; \
    }

#define CALL_WASM_FUNC(func_ptr) \
    if (!wasm_runtime_call_wasm(exec_env, func_ptr, func_argc, func_argv) ) { \
        snprintf(output_buff, sizeof(output_buff), "call wasm function "#func_ptr" failed. %s\n", wasm_runtime_get_exception(module_inst)); \
        ocall_print(output_buff); \
        return; \
    }


extern "C" {
    typedef void (*os_print_function_t)(const char* message);
    extern void os_set_print_function(os_print_function_t pf);

    void enclave_print(const char *message)
    {
        ocall_print(message);
    }

    clock_t enclave_clock(void)
    {
        clock_t time;
        ocall_clock(&time);
        return time;
    }
}


void perform_benchmark(database_type_t database_type, int number_of_write, int number_of_read, int must_print_memory_usage)
{
    char output_buff[256];
    clock_t start, end;

    // Resolve benchmark functions
    wasm_function_inst_t init_rand = NULL;
    wasm_function_inst_t open_db = NULL;
    wasm_function_inst_t setup_database = NULL;
    wasm_function_inst_t insert_data = NULL;
    wasm_function_inst_t query_data_sequential = NULL;
    wasm_function_inst_t query_data_random = NULL;
    wasm_function_inst_t close_db = NULL;
    wasm_function_inst_t print_memory_usage = NULL;

    LOOKUP_WASM_FUNC(init_rand);
    LOOKUP_WASM_FUNC(open_db);
    LOOKUP_WASM_FUNC(setup_database);
    LOOKUP_WASM_FUNC(insert_data);
    LOOKUP_WASM_FUNC(query_data_sequential);
    LOOKUP_WASM_FUNC(query_data_random);
    LOOKUP_WASM_FUNC(close_db);
    LOOKUP_WASM_FUNC(print_memory_usage);

    // Call the functions to perform the benchmark
    // Declare the buffer for the arguments passing
    unsigned int func_argc = 0;
    unsigned int func_argv[2];

    // Initialize the PRNG
    CALL_WASM_FUNC(init_rand);

    // Open the database with the given type
    func_argc = 1;
    func_argv[0] = database_type;
    func_argv[1] = number_of_write;
    CALL_WASM_FUNC(open_db);
    
    // Set up the database
    func_argc = 0;
    CALL_WASM_FUNC(setup_database);

    // Insert the given amount of data in the database
    func_argc = 1;
    func_argv[0] = number_of_write;

    start = enclave_clock();
    CALL_WASM_FUNC(insert_data);
    end = enclave_clock();

    DISPLAY_TIME("i")

    // Query sequentially the given amount of data in the database
    func_argc = 2;
    func_argv[0] = number_of_write;
    func_argv[1] = number_of_read;
    
    start = enclave_clock();
    CALL_WASM_FUNC(query_data_sequential);
    end = enclave_clock();

    DISPLAY_TIME("qs")

    // Query randomly the given amount of data in the database
    start = enclave_clock();
    CALL_WASM_FUNC(query_data_random);
    end = enclave_clock();

    DISPLAY_TIME("qr")

    // Display the memory usage of SQLite if requested
    if (must_print_memory_usage)
    {
        func_argc = 0;
        CALL_WASM_FUNC(print_memory_usage);
    }

    // Close the database
    func_argc = 0;
    CALL_WASM_FUNC(close_db);
}

void ecall_benchmark(uint8_t* wasm_buffer, size_t wasm_buffer_len, int database_type, int number_of_write, int number_of_read, int must_print_memory_usage)
{
    os_set_print_function(enclave_print);
    
    uint8_t *wasm_file_buf = NULL;
    int wasm_file_size;
    wasm_module_t wasm_module = NULL;
    char error_buf[128];

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    void* global_heap_buf = malloc(GLOBAL_HEAP_SIZE);
    memset(global_heap_buf, 0, GLOBAL_HEAP_SIZE);

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = GLOBAL_HEAP_SIZE;

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        ocall_print("Init runtime environment failed.");
        ocall_print("\n");
        return;
    }

    // Set empty argc and argv
    char *wasm_argv[] = { NULL };
    uint32_t wasm_argc = 0;

    // Define the directories that the WASI calls can interact with
    const char *dir_list[] = { ".", NULL };
    uint32_t dir_list_size = 1;

    // Define the environment variables that the WASI calls can query
    const char *env_list[] = { NULL };
    uint32_t env_list_size = 0;

    /* load WASM byte buffer from the argument */
    wasm_file_buf = wasm_buffer;
    wasm_file_size = wasm_buffer_len;

    /* load WASM module */
    if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size,
                                          error_buf, sizeof(error_buf)))) {
        ocall_print(error_buf);
        ocall_print("\n");
        goto fail1;
    }

    wasm_runtime_set_wasi_args(wasm_module, dir_list, dir_list_size, NULL, 0,
                               env_list, env_list_size, wasm_argv, wasm_argc);

    /* instantiate the module */
    if (!(module_inst = wasm_runtime_instantiate(wasm_module,
                                                      STACK_SIZE,
                                                      HEAP_SIZE,
                                                      error_buf,
                                                      sizeof(error_buf)))) {
        ocall_print(error_buf);
        ocall_print("\n");
        goto fail2;
    }
    
    exec_env = wasm_runtime_create_exec_env(module_inst, STACK_SIZE);
    if(!exec_env) {
        ocall_print("Create wasm execution environment failed.\n");
        goto fail2;
    }

    /* execute the main function of wasm app */
    wasm_application_execute_main(module_inst, wasm_argc, wasm_argv);

    const char *exception;
    if ((exception = wasm_runtime_get_exception(module_inst))) {
        ocall_print(exception);
        ocall_print("\n");
    }

    // Perform the benchmark
    perform_benchmark((database_type_t)database_type, number_of_write, number_of_read, must_print_memory_usage);

    /* destroy the module instance */
    wasm_runtime_deinstantiate(module_inst);

fail2:
    /* unload the module */
    wasm_runtime_unload(wasm_module);

fail1:
    /* destroy runtime environment */
    wasm_runtime_destroy();

    free(global_heap_buf);
}