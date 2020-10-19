#ifndef WASI_API_FD_SGX_H
#define WASI_API_FD_SGX_H

#include "wasi_api_fd.h"

#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2
#define SGX_ERROR_FILE_BASE 0x7000
#define IS_STD_STREAM(fd) fd <= FD_STDERR
#define IS_NOT_STD_STREAM(fd) fd > FD_STDERR

// Converts a POSIX + SGX error code to a CloudABI error code.
__wasi_errno_t convert_file_errno(int error);

// Returns the underlying SGX file descriptor number of a file descriptor
// object. This function can only be applied to objects that have an
// underlying file descriptor number.
void* fd_number_sgx(
    const struct fd_object *fo
);

// Inserts a SGX file descriptor into the file descriptor table.
__wasi_errno_t fd_table_insert_fd_sgx(
    struct fd_table *ft,
    void* in,
    __wasi_filetype_t type,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    __wasi_fd_t *out,
    const char *path, // Added for SGX PFS
    size_t pathlen // Added for SGX PFS
) REQUIRES_UNLOCKED(ft->lock);

#endif /* WASI_API_FD_SGX_H */