// Part of the Wasmtime Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://github.com/CraneStation/wasmtime/blob/master/LICENSE for license information.
//
// Significant parts of this file are derived from cloudabi-utils. See
// https://github.com/CraneStation/wasmtime/blob/master/lib/wasi/sandboxed-system-primitives/src/LICENSE
// for license information.
//
// The upstream file contains the following copyright notice:
//
// Copyright (c) 2016-2018 Nuxi, https://nuxi.nl/

#ifndef WASI_INTERNAL_H
#define WASI_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include "wasi_api_fd.h"

struct argv_environ_values {
  size_t argc;
  size_t argv_buf_size;
  char **argv;
  char *argv_buf;
  size_t environ_count;
  size_t environ_buf_size;
  char **environ;
  char *environ_buf;
};

#endif /* WASI_INTERNAL_H */
