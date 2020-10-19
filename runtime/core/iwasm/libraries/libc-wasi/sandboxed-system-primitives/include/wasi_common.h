#ifndef WASI_COMMON_H
#define WASI_COMMON_H

#include "wasmtime_ssp.h"
#include "wasi_internal.h"
#include "wasi_api_fd.h"

void fd_table_init(struct fd_table *);
bool fd_table_insert_existing(struct fd_table *, __wasi_fd_t, int);
void fd_prestats_init(struct fd_prestats *);
bool fd_prestats_insert(struct fd_prestats *, const char *, __wasi_fd_t);
bool argv_environ_init(struct argv_environ_values *,
                       const size_t *argv_offsets, size_t argv_offsets_len,
                       const char *argv_buf, size_t argv_buf_len,
                       const size_t *environ_offsets, size_t environ_offsets_len,
                       const char *environ_buf, size_t environ_buf_len);
void argv_environ_destroy(struct argv_environ_values *argv_environ);
void fd_table_destroy(struct fd_table *ft);
void fd_prestats_destroy(struct fd_prestats *pt);

#endif /* WASI_COMMON_H */