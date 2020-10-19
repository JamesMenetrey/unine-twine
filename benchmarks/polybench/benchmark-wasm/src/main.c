
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "wasm_export.h"
#include "bh_read_file.h"

#define GLOBAL_HEAP_SIZE (160 * 1024 * 1024)
#define HEAP_SIZE (1024 * 1024 * 4)
#define STACK_SIZE (1024 * 1024 * 5)

#define LOOKUP_WASM_FUNC(func_name) \
    wasm_function_inst_t ptr_##func_name = NULL; \
    if(!(ptr_##func_name = wasm_runtime_lookup_function(module_inst, #func_name, NULL))){ \
        printf("The "#func_name" wasm function is not found.\n"); \
        return; \
    }

#define CALL_WASM_FUNC(func_ptr) \
    if (!wasm_runtime_call_wasm(exec_env, ptr_##func_ptr, func_argc, func_argv) ) { \
        printf("call wasm function "#func_ptr" failed. %s\n", wasm_runtime_get_exception(module_inst)); \
        return; \
    }

void perform_benchmark(
    const wasm_exec_env_t exec_env,
    const wasm_module_inst_t module_inst)
{
    clock_t start, end;

    // Call the main function of the WASM
    int wasm_argc = 0;
    char *wasm_argv[] = { NULL };
    wasm_application_execute_main(module_inst, wasm_argc, wasm_argv);
    
    // Resolve benchmark functions
    LOOKUP_WASM_FUNC(benchmark);

    // Call the functions to perform the benchmark
    // Declare the buffer for the arguments passing
    unsigned int func_argc = 0;
    unsigned int func_argv[0];

    start = clock();
    CALL_WASM_FUNC(benchmark);
    end = clock();

    printf("%ld\n", end - start);
}

void get_resource_path(const char* resource_name, const char* argv0, char* buffer, size_t buffer_len)
{
    snprintf(buffer, buffer_len, "%.*s/%s", (int)(strrchr(argv0, '/') - argv0), argv0, resource_name);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf ("Usage: path-to-benchmark-file-aot\n");
        return 1;
    }

    char *buffer, error_buf[128];
    char *wasm_path = argv[1];

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

    perform_benchmark(exec_env, module_inst);
    
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
