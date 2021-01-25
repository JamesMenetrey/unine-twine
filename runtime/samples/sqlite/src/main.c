
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "wasm_export.h"
#include "bh_read_file.h"

int main(int argc, char *argv[])
{
    bh_log_set_verbose_level(BH_LOG_LEVEL_VERBOSE);
    static char global_heap_buf[50 * 1024 * 1024]; // TODO Fine-tuning required
    char *buffer, error_buf[128];
    char * wasm_path = "wasm-apps/sqlite-test.wasm";

    wasm_module_t module = NULL;
    wasm_module_inst_t module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    uint32 buf_size, stack_size = 8092 * 1024, heap_size = 8092 * 1024; // TODO Fine-tuning required

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        return -1;
    }

    buffer = bh_read_file_to_buffer(wasm_path, &buf_size);

    // Set empty argc and argv
    char *wasm_argv[] = { NULL };
    uint32_t wasm_argc = 0;

    // Define the directories that the WASI calls can interact with
    const char *dir_list[] = { ".", NULL };
    uint32_t dir_list_size = 1;

    // Define the environment variables that the WASI calls can query
    const char *env_list[] = { NULL };
    uint32_t env_list_size = 0;

    if(!buffer) {
        printf("Open wasm app file [%s] failed.\n", wasm_path);
        goto fail;
    }

    module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));
    if(!module) {
        printf("Load wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    wasm_runtime_set_wasi_args(module, dir_list, dir_list_size, NULL, 0,
                               env_list, env_list_size, wasm_argv, wasm_argc);

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

    // Call the main function of the WASM
    wasm_application_execute_main(module_inst, wasm_argc, wasm_argv);
    
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
