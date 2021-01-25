#ifndef WASI_INTERNAL_H
#define WASI_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include "bh_common.h"
#include "wasmtime_ssp.h"
#include "_atomic.h"
#include "_locking.h"

#if WAMR_BUILD_SGX_DISABLE_UNTRUSTED_CALLS
#define ENSURE_UNTRUSTED_CALL_ENABLED(wasi_call) \
  os_printf("The WASI function "#wasi_call" cannot not be called because Twine is compiled with the flag enforcing trusted POSIX implementation only.\n"); \
  return __WASI_ECANCELED;
#else
#define ENSURE_UNTRUSTED_CALL_ENABLED(wasi_call)
#endif

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