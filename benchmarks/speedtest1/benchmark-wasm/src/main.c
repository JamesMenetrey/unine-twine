
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "wasm_export.h"
#include "bh_read_file.h"

#define GLOBAL_HEAP_SIZE (60 * 1024 * 1024)
#define HEAP_SIZE (1024 * 1024 * 4)
#define STACK_SIZE (1024 * 1024 * 5)

#define LOOKUP_WASM_FUNC(func_name) \
    wasm_function_inst_t ptr_##func_name = NULL; \
    if(!(ptr_##func_name = wasm_runtime_lookup_function(module_inst, #func_name, NULL))){ \
        printf("The "#func_name" wasm function is not found.\n"); \
        return; \
    }

#define CALL_WASM_FUNC(func_ptr) \
    start = clock(); \
    if (!wasm_runtime_call_wasm(exec_env, ptr_##func_ptr, func_argc, func_argv) ) { \
        printf("call wasm function "#func_ptr" failed. %s\n", wasm_runtime_get_exception(module_inst)); \
        return; \
    } \
    clock_t t_##func_ptr = clock() - start;

void perform_benchmark(
    const wasm_exec_env_t exec_env,
    const wasm_module_inst_t module_inst,
    const int database_type)
{
    clock_t start;

    // Call the main function of the WASM
    int wasm_argc = 0;
    char *wasm_argv[] = { NULL };
    wasm_application_execute_main(module_inst, wasm_argc, wasm_argv);
    
    // Resolve benchmark functions
    LOOKUP_WASM_FUNC(speedtest1_initialize);
    LOOKUP_WASM_FUNC(speedtest1_launch100)
    LOOKUP_WASM_FUNC(speedtest1_launch110)
    LOOKUP_WASM_FUNC(speedtest1_launch120)
    LOOKUP_WASM_FUNC(speedtest1_launch130)
    LOOKUP_WASM_FUNC(speedtest1_launch140)
    LOOKUP_WASM_FUNC(speedtest1_launch142)
    LOOKUP_WASM_FUNC(speedtest1_launch145)
    LOOKUP_WASM_FUNC(speedtest1_launch160)
    LOOKUP_WASM_FUNC(speedtest1_launch161)
    LOOKUP_WASM_FUNC(speedtest1_launch170)
    LOOKUP_WASM_FUNC(speedtest1_launch180)
    LOOKUP_WASM_FUNC(speedtest1_launch190)
    LOOKUP_WASM_FUNC(speedtest1_launch210)
    LOOKUP_WASM_FUNC(speedtest1_launch230)
    LOOKUP_WASM_FUNC(speedtest1_launch240)
    LOOKUP_WASM_FUNC(speedtest1_launch250)
    LOOKUP_WASM_FUNC(speedtest1_launch260)
    LOOKUP_WASM_FUNC(speedtest1_launch270)
    LOOKUP_WASM_FUNC(speedtest1_launch280)
    LOOKUP_WASM_FUNC(speedtest1_launch290)
    LOOKUP_WASM_FUNC(speedtest1_launch300)
    LOOKUP_WASM_FUNC(speedtest1_launch320)
    LOOKUP_WASM_FUNC(speedtest1_launch400)
    LOOKUP_WASM_FUNC(speedtest1_launch410)
    LOOKUP_WASM_FUNC(speedtest1_launch500)
    LOOKUP_WASM_FUNC(speedtest1_launch510)
    LOOKUP_WASM_FUNC(speedtest1_launch520)
    LOOKUP_WASM_FUNC(speedtest1_launch980)
    LOOKUP_WASM_FUNC(speedtest1_launch990)

    // Call the functions to perform the benchmark
    // Declare the buffer for the arguments passing
    unsigned int func_argc = 2;
    unsigned int func_argv[2];

    // Initialize the speedtest1
    func_argc = 1;
    func_argv[0] = database_type;
    CALL_WASM_FUNC(speedtest1_initialize);
    if (t_speedtest1_initialize == 0){
        printf("error during initialization.\n");
        return;
    }

    // Execute the speed tests
    func_argc = 0;

    CALL_WASM_FUNC(speedtest1_launch100)
    CALL_WASM_FUNC(speedtest1_launch110)
    CALL_WASM_FUNC(speedtest1_launch120)
    CALL_WASM_FUNC(speedtest1_launch130)
    CALL_WASM_FUNC(speedtest1_launch140)
    CALL_WASM_FUNC(speedtest1_launch142)
    CALL_WASM_FUNC(speedtest1_launch145)
    CALL_WASM_FUNC(speedtest1_launch160)
    CALL_WASM_FUNC(speedtest1_launch161)
    CALL_WASM_FUNC(speedtest1_launch170)
    CALL_WASM_FUNC(speedtest1_launch180)
    CALL_WASM_FUNC(speedtest1_launch190)
    CALL_WASM_FUNC(speedtest1_launch210)
    CALL_WASM_FUNC(speedtest1_launch230)
    CALL_WASM_FUNC(speedtest1_launch240)
    CALL_WASM_FUNC(speedtest1_launch250)
    CALL_WASM_FUNC(speedtest1_launch260)
    CALL_WASM_FUNC(speedtest1_launch270)
    CALL_WASM_FUNC(speedtest1_launch280)
    CALL_WASM_FUNC(speedtest1_launch290)
    CALL_WASM_FUNC(speedtest1_launch300)
    CALL_WASM_FUNC(speedtest1_launch320)
    CALL_WASM_FUNC(speedtest1_launch400)
    CALL_WASM_FUNC(speedtest1_launch410)
    CALL_WASM_FUNC(speedtest1_launch500)
    CALL_WASM_FUNC(speedtest1_launch510)
    CALL_WASM_FUNC(speedtest1_launch520)
    CALL_WASM_FUNC(speedtest1_launch980)
    CALL_WASM_FUNC(speedtest1_launch990)

    printf("%d, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n",
        database_type, t_speedtest1_launch100, t_speedtest1_launch110, t_speedtest1_launch120, t_speedtest1_launch130, t_speedtest1_launch140, t_speedtest1_launch142, t_speedtest1_launch145,
        t_speedtest1_launch160, t_speedtest1_launch161, t_speedtest1_launch170, t_speedtest1_launch180, t_speedtest1_launch190, t_speedtest1_launch210,t_speedtest1_launch230, 
        t_speedtest1_launch240, t_speedtest1_launch250, t_speedtest1_launch260, t_speedtest1_launch270, t_speedtest1_launch280, t_speedtest1_launch290, t_speedtest1_launch300, 
        t_speedtest1_launch320, t_speedtest1_launch400, t_speedtest1_launch410, t_speedtest1_launch500, t_speedtest1_launch510, t_speedtest1_launch520, t_speedtest1_launch980,
        t_speedtest1_launch990);
}

void get_resource_path(const char* resource_name, const char* argv0, char* buffer, size_t buffer_len)
{
    snprintf(buffer, buffer_len, "%.*s/%s", (int)(strrchr(argv0, '/') - argv0), argv0, resource_name);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf ("Usage: [0(in-memory)|1(in-file)]\n");
        return 1;
    }

    int database_type = strtol(argv[1], NULL, 10);
    char *buffer, error_buf[128];
    char wasm_path[200];

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

    perform_benchmark(exec_env, module_inst, database_type);
    
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
