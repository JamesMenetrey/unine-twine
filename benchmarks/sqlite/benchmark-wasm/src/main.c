
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "wasm_export.h"
#include "bh_read_file.h"
#include "benchmark.h"

#define GLOBAL_HEAP_SIZE (240 * 1024 * 1024)
#define HEAP_SIZE (1024 * 1024 * 4)
#define STACK_SIZE (1024 * 1024 * 5)

#define LOOKUP_WASM_FUNC(func_name) \
    if(!(func_name = wasm_runtime_lookup_function(module_inst, #func_name, NULL))){ \
        printf("The "#func_name" wasm function is not found.\n"); \
        return; \
    }

#define CALL_WASM_FUNC(func_ptr) \
    if (!wasm_runtime_call_wasm(exec_env, func_ptr, func_argc, func_argv) ) { \
        printf("call wasm function "#func_ptr" failed. %s\n", wasm_runtime_get_exception(module_inst)); \
        return; \
    }

void perform_benchmark(
    const wasm_exec_env_t exec_env,
    const wasm_module_inst_t module_inst,
    const int number_of_write,
    const int number_of_read,
    const database_type_t database_type,
    const int must_print_memory_usage)
{
    // Call the main function of the WASM
    clock_t start, end;
    int wasm_argc = 0;
    char *wasm_argv[] = { NULL };
    wasm_application_execute_main(module_inst, wasm_argc, wasm_argv);

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
    func_argc = 2;
    func_argv[0] = database_type;
    func_argv[1] = number_of_write;
    CALL_WASM_FUNC(open_db);

    // Set up the database
    func_argc = 0;
    CALL_WASM_FUNC(setup_database);

    // Insert the given amount of data in the database
    func_argc = 1;
    func_argv[0] = number_of_write;

    start = clock();
    CALL_WASM_FUNC(insert_data);
    end = clock();

    printf("i,%d,%d,%d,%ld\n", database_type, number_of_write, number_of_read, end - start);

    // Query the given amount of data in the database
    func_argc = 2;
    func_argv[0] = number_of_write;
    func_argv[1] = number_of_read;

    start = clock();
    CALL_WASM_FUNC(query_data_sequential);
    end = clock();

    printf("qs,%d,%d,%d,%ld\n", database_type, number_of_write, number_of_read, end - start);

    start = clock();
    CALL_WASM_FUNC(query_data_random);
    end = clock();

    printf("qr,%d,%d,%d,%ld\n", database_type, number_of_write, number_of_read, end - start);

    if (must_print_memory_usage)
    {
        func_argc = 0;
        CALL_WASM_FUNC(print_memory_usage);
    }

    // Close the database
    func_argc = 0;
    CALL_WASM_FUNC(close_db);
}

void get_resource_path(const char* resource_name, const char* argv0, char* buffer, size_t buffer_len)
{
    snprintf(buffer, buffer_len, "%.*s/%s", (int)(strrchr(argv0, '/') - argv0), argv0, resource_name);
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf ("Usage: [0(in-memory)|1(in-file)] number_of_write number_of_read print_memory_usage profiling_level\n");
        return 1;
    }

    database_type_t database_type = (database_type_t)strtol(argv[1], NULL, 10);
    int number_of_write = strtol(argv[2], NULL, 10);
    int number_of_read = strtol(argv[3], NULL, 10);
    int must_print_memory_usage = strtol(argv[4], NULL, 10);
    int profiling_level = strtol(argv[5], NULL, 10);
    char *buffer, error_buf[128];
    char wasm_path[200];

    if (profiling_level != 0)
    {
        printf("Profiling is not supported.\n");
        return 1;
    }

    get_resource_path("benchmark-wasm.aot", argv[0], wasm_path, sizeof(wasm_path));

    wasm_module_t module = NULL;
    wasm_module_inst_t module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    uint32 buf_size, stack_size = STACK_SIZE, heap_size = HEAP_SIZE;

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    void* global_heap_buf = malloc(GLOBAL_HEAP_SIZE);
    memset(global_heap_buf, 0, GLOBAL_HEAP_SIZE);

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = GLOBAL_HEAP_SIZE;

    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        return -1;
    }

    buffer = bh_read_file_to_buffer(wasm_path, &buf_size);

    if(!buffer) {
        printf("Open wasm app file [%s] failed.\n", wasm_path);
        goto fail;
    }

    module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));
    if(!module) {
        printf("Load wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    // Define the directories that the WASI calls can interact with
    const char *dir_list[] = { ".", NULL };
    uint32 dir_list_size = 1;

    // Define the environment variables that the WASI calls can query
    const char *env_list[] = { NULL };
    uint32 env_list_size = 0;

    wasm_runtime_set_wasi_args(module, dir_list, dir_list_size, NULL, 0,
                               env_list, env_list_size, argv, argc);
                               
    module_inst = wasm_runtime_instantiate(module,
                                         stack_size,
                                         heap_size,
                                         error_buf,
                                         sizeof(error_buf));

    if(!module_inst) {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if(!exec_env) {
        printf("Create wasm execution environment failed.\n");
        goto fail;
    }

    perform_benchmark(exec_env, module_inst, number_of_write, number_of_read, database_type, must_print_memory_usage);
    
fail:
    if(exec_env) wasm_runtime_destroy_exec_env(exec_env);
    if(module_inst) {
        wasm_runtime_deinstantiate(module_inst);
    }
    if(module) wasm_runtime_unload(module);
    if(buffer) BH_FREE(buffer);
    wasm_runtime_destroy();
    return 0;
}
