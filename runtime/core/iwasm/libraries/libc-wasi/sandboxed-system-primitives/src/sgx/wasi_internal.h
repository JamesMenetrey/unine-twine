#ifndef WASI_INTERNAL_H
#define WASI_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include "bh_common.h"
#include "wasmtime_ssp.h"
#include "_atomic.h"
#include "_locking.h"

//
// Definition of missing types
//
typedef uint32_t uint32;
typedef uint64_t uint64;

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