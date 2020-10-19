#ifndef WASI_API_FD_H
#define WASI_API_FD_H

#include "wasmtime_ssp.h"
#include "wasi_common.h"
#include "wasi_api_fd_internal.h"
#include "common/time.h"

//
// Abstract
//

// Lowers the reference count on a file descriptor object. When the
// reference count reaches zero, its resources are cleaned up.
void fd_object_release(
    struct fd_object *fo
) UNLOCKS(fo->refcount);

void path_put(
    struct path_access *pa
) UNLOCKS(pa->fd_object->refcount);

// Determines the type of a file descriptor and its maximum set of
// rights that should be attached to it.
__wasi_errno_t fd_determine_type_rights(
    int fd,
    __wasi_filetype_t *type,
    __wasi_rights_t *rights_base,
    __wasi_rights_t *rights_inheriting
);

// Picks an unused slot from the file descriptor table.
__wasi_fd_t fd_table_unused(
    struct fd_table *ft
) REQUIRES_SHARED(ft->lock);

// Creates a lease to a file descriptor and pathname pair. If the
// operating system does not implement Capsicum, it also normalizes the
// pathname to ensure the target path is placed underneath the
// directory.
__wasi_errno_t path_get(
    struct fd_table *curfds,
    struct path_access *pa,
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *upath,
    size_t upathlen,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    bool needs_final_component
) TRYLOCKS_EXCLUSIVE(0, pa->fd_object->refcount);

//
// Implemented
//

// Converts a POSIX error code to a CloudABI error code.
__wasi_errno_t convert_errno(
    int error
);

// Converts a POSIX stat structure to a CloudABI filestat structure.
void convert_stat(
    const struct stat *in,
    __wasi_filestat_t *out
);

// Returns the underlying file descriptor number of a file descriptor
// object. This function can only be applied to objects that have an
// underlying file descriptor number.
int fd_number(
    const struct fd_object *fo
);

// Temporarily locks the file descriptor table to look up a file
// descriptor object, increases its reference count and drops the lock.
__wasi_errno_t fd_object_get(
    struct fd_table *curfds,
    struct fd_object **fo,
    __wasi_fd_t fd,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting
) TRYLOCKS_EXCLUSIVE(0, (*fo)->refcount);

// Look up a file descriptor object in a locked file descriptor table
// and increases its reference count.
__wasi_errno_t fd_object_get_locked(
    struct fd_object **fo,
    struct fd_table *ft,
    __wasi_fd_t fd,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting
) TRYLOCKS_EXCLUSIVE(0, (*fo)->refcount) REQUIRES_EXCLUSIVE(ft->lock);

// Allocates a new file descriptor object.
__wasi_errno_t fd_object_new(
    __wasi_filetype_t type,
    struct fd_object **fo
) TRYLOCKS_SHARED(0, (*fo)->refcount);

// Attaches a file descriptor to the file descriptor table.
void fd_table_attach(
    struct fd_table *ft,
    __wasi_fd_t fd,
    struct fd_object *fo,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting
) REQUIRES_EXCLUSIVE(ft->lock) CONSUMES(fo->refcount);

// Detaches a file descriptor from the file descriptor table.
void fd_table_detach(
    struct fd_table *ft,
    __wasi_fd_t fd,
    struct fd_object **fo
) REQUIRES_EXCLUSIVE(ft->lock) PRODUCES((*fo)->refcount);

// Looks up a file descriptor table entry by number and required rights.
__wasi_errno_t fd_table_get_entry(
    struct fd_table *ft,
    __wasi_fd_t fd,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    struct fd_entry **ret
) REQUIRES_SHARED(ft->lock);

// Grows the file descriptor table to a required lower bound and a
// minimum number of free file descriptor table entries.
bool fd_table_grow(
    struct fd_table *ft,
    size_t min,
    size_t incr
) REQUIRES_EXCLUSIVE(ft->lock);

// Inserts a file descriptor object into an unused slot of the file
// descriptor table.
__wasi_errno_t fd_table_insert(
    struct fd_table *ft,
    struct fd_object *fo,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    __wasi_fd_t *out
) REQUIRES_UNLOCKED(ft->lock) UNLOCKS(fo->refcount);

#endif /* WASI_API_FD_H */