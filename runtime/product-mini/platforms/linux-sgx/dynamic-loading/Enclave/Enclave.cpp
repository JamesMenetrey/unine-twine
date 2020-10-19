/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Enclave_t.h"
#include "wasm_export.h"

//static char global_heap_buf[50 * 1024 * 1024] = { 0 }; // TODO Fine-tuning required
static uint32_t buf_size, stack_size = 1024 * 1024, heap_size = 1024 * 1024 * 2;

static int app_argc;
static char **app_argv;

static void* app_instance_main(wasm_module_inst_t module_inst)
{
    const char *exception;

    wasm_application_execute_main(module_inst, app_argc, app_argv);
    if ((exception = wasm_runtime_get_exception(module_inst))) {
        ocall_print(exception);
        ocall_print("\n");
    }
    return NULL;
}

extern "C" {
    typedef void (*os_print_function_t)(const char* message);
    extern void os_set_print_function(os_print_function_t pf);

    void enclave_print(const char *message)
    {
        ocall_print(message);
    }
    
    typedef int (*os_clock_gettime_t)(clockid_t clk_id, struct timespec *ts);
    extern void os_set_clock_gettime(os_clock_gettime_t pf);

    int enclave_clock_gettime(clockid_t clk_id, struct timespec *ts)
    {
        int retval;
        ocall_clock_gettime(&retval, clk_id, ts);
        return retval;
    }

    typedef clock_t (*os_clock_t)();
    void os_set_clock(os_clock_t pf);

    clock_t enclave_clock(void)
    {
        clock_t time;
        ocall_clock(&time);
        return time;
    }
}

// The first parameter exec_env must be defined using type wasm_exec_env_t
// which is the calling convention for exporting native API by WAMR.
void unine_print_native(wasm_exec_env_t exec_env, char* columnName, char* value)
{
    ocall_print(columnName);
    ocall_print(" = ");
    ocall_print(value);
    ocall_print("\n");
}

void ecall_load_wasm(uint8_t* wasm_buffer, size_t wasm_buffer_len)
{
    os_set_print_function(enclave_print);
    os_set_clock_gettime(enclave_clock_gettime);
    os_set_clock(enclave_clock);

    uint8_t *wasm_file_buf = NULL;
    int wasm_file_size;
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    RuntimeInitArgs init_args;
    char error_buf[128];

    static NativeSymbol native_symbols[] =
    {
        {
            "unine_print", 		                            // the name of WASM function name
            reinterpret_cast<void*>(unine_print_native),    // the native function pointer
            "($$)",			                                // the function prototype signature, avoid to use i32
            NULL                                            // attachment is NULL
        }
    };

    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type = Alloc_With_System_Allocator;
    //init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    //init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
    
    // Native symbols need below registration phase
    init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
    init_args.native_module_name = "env";
    init_args.native_symbols = native_symbols;
    
    const char* dir_list[] = { "." };
    int dir_list_size = 1;

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        ocall_print("Init runtime environment failed.");
        ocall_print("\n");
        return;
    }

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
                               NULL, 0, NULL, 0);

    /* instantiate the module */
    if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module,
                                                      stack_size,
                                                      heap_size,
                                                      error_buf,
                                                      sizeof(error_buf)))) {
        ocall_print(error_buf);
        ocall_print("\n");
        goto fail2;
    }

    /* execute the main function of wasm app */
    app_instance_main(wasm_module_inst);

    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);

fail2:
    /* unload the module */
    wasm_runtime_unload(wasm_module);

fail1:
    /* destroy runtime environment */
    wasm_runtime_destroy();
}