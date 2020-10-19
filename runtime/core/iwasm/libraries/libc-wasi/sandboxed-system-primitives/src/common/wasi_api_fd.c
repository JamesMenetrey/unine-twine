
#include "wasmtime_ssp.h"
#include "bh_common.h"
#include "bh_assert.h"
#include "wasi_api_fd.h"

// Converts a POSIX error code to a CloudABI error code.
__wasi_errno_t convert_errno(int error) {
  static const __wasi_errno_t errors[] = {
#define X(v) [v] = __WASI_##v
    X(E2BIG),
    X(EACCES),
    X(EADDRINUSE),
    X(EADDRNOTAVAIL),
    X(EAFNOSUPPORT),
    X(EAGAIN),
    X(EALREADY),
    X(EBADF),
    X(EBADMSG),
    X(EBUSY),
    X(ECANCELED),
    X(ECHILD),
    X(ECONNABORTED),
    X(ECONNREFUSED),
    X(ECONNRESET),
    X(EDEADLK),
    X(EDESTADDRREQ),
    X(EDOM),
    X(EDQUOT),
    X(EEXIST),
    X(EFAULT),
    X(EFBIG),
    X(EHOSTUNREACH),
    X(EIDRM),
    X(EILSEQ),
    X(EINPROGRESS),
    X(EINTR),
    X(EINVAL),
    X(EIO),
    X(EISCONN),
    X(EISDIR),
    X(ELOOP),
    X(EMFILE),
    X(EMLINK),
    X(EMSGSIZE),
    X(EMULTIHOP),
    X(ENAMETOOLONG),
    X(ENETDOWN),
    X(ENETRESET),
    X(ENETUNREACH),
    X(ENFILE),
    X(ENOBUFS),
    X(ENODEV),
    X(ENOENT),
    X(ENOEXEC),
    X(ENOLCK),
    X(ENOLINK),
    X(ENOMEM),
    X(ENOMSG),
    X(ENOPROTOOPT),
    X(ENOSPC),
    X(ENOSYS),
#ifdef ENOTCAPABLE
    X(ENOTCAPABLE),
#endif
    X(ENOTCONN),
    X(ENOTDIR),
    X(ENOTEMPTY),
    X(ENOTRECOVERABLE),
    X(ENOTSOCK),
    X(ENOTSUP),
    X(ENOTTY),
    X(ENXIO),
    X(EOVERFLOW),
    X(EOWNERDEAD),
    X(EPERM),
    X(EPIPE),
    X(EPROTO),
    X(EPROTONOSUPPORT),
    X(EPROTOTYPE),
    X(ERANGE),
    X(EROFS),
    X(ESPIPE),
    X(ESRCH),
    X(ESTALE),
    X(ETIMEDOUT),
    X(ETXTBSY),
    X(EXDEV),
#undef X
#if EOPNOTSUPP != ENOTSUP
    [EOPNOTSUPP] = __WASI_ENOTSUP,
#endif
#if EWOULDBLOCK != EAGAIN
    [EWOULDBLOCK] = __WASI_EAGAIN,
#endif
  };
  if (error < 0 || (size_t)error >= sizeof(errors) / sizeof(errors[0]) ||
      errors[error] == 0)
    return __WASI_ENOSYS;
  return errors[error];
}

// Converts a POSIX stat structure to a CloudABI filestat structure.
void convert_stat(
    const struct stat *in,
    __wasi_filestat_t *out
) {
  *out = (__wasi_filestat_t){
      .st_dev = in->st_dev,
      .st_ino = in->st_ino,
      .st_nlink = (__wasi_linkcount_t)in->st_nlink,
      .st_size = (__wasi_filesize_t)in->st_size,
      .st_atim = convert_timespec(&in->st_atim),
      .st_mtim = convert_timespec(&in->st_mtim),
      .st_ctim = convert_timespec(&in->st_ctim),
  };
}

// Returns the underlying file descriptor number of a file descriptor
// object. This function can only be applied to objects that have an
// underlying file descriptor number.
int fd_number(
    const struct fd_object *fo
) {
  int number = fo->number;
  assert(number >= 0 && "fd_number() called on virtual file descriptor");
  return number;
}

// Temporarily locks the file descriptor table to look up a file
// descriptor object, increases its reference count and drops the lock.
__wasi_errno_t fd_object_get(
    struct fd_table *curfds,
    struct fd_object **fo,
    __wasi_fd_t fd,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting
) TRYLOCKS_EXCLUSIVE(0, (*fo)->refcount) {
  struct fd_table *ft = curfds;
  rwlock_rdlock(&ft->lock);
  __wasi_errno_t error =
      fd_object_get_locked(fo, ft, fd, rights_base, rights_inheriting);
  rwlock_unlock(&ft->lock);
  return error;
}

// Look up a file descriptor object in a locked file descriptor table
// and increases its reference count.
__wasi_errno_t fd_object_get_locked(
    struct fd_object **fo,
    struct fd_table *ft,
    __wasi_fd_t fd,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting
) TRYLOCKS_EXCLUSIVE(0, (*fo)->refcount) REQUIRES_EXCLUSIVE(ft->lock) {
  // Test whether the file descriptor number is valid.
  struct fd_entry *fe;
  __wasi_errno_t error =
      fd_table_get_entry(ft, fd, rights_base, rights_inheriting, &fe);
  if (error != 0)
    return error;

  // Increase the reference count on the file descriptor object. A copy
  // of the rights are also stored, so callers can still access those if
  // needed.
  *fo = fe->object;
  refcount_acquire(&(*fo)->refcount);
  return 0;
}

// Allocates a new file descriptor object.
__wasi_errno_t fd_object_new(
    __wasi_filetype_t type,
    struct fd_object **fo
) TRYLOCKS_SHARED(0, (*fo)->refcount) {
  *fo = wasm_runtime_malloc(sizeof(**fo));
  if (*fo == NULL)
    return __WASI_ENOMEM;
  refcount_init(&(*fo)->refcount, 1);
  (*fo)->type = type;
  (*fo)->number = -1;
  return 0;
}

// Attaches a file descriptor to the file descriptor table.
void fd_table_attach(
    struct fd_table *ft,
    __wasi_fd_t fd,
    struct fd_object *fo,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting
) REQUIRES_EXCLUSIVE(ft->lock) CONSUMES(fo->refcount) {
  assert(ft->size > fd && "File descriptor table too small");
  struct fd_entry *fe = &ft->entries[fd];
  assert(fe->object == NULL && "Attempted to overwrite an existing descriptor");
  fe->object = fo;
  fe->rights_base = rights_base;
  fe->rights_inheriting = rights_inheriting;
  ++ft->used;
  assert(ft->size >= ft->used * 2 && "File descriptor too full");
}

// Detaches a file descriptor from the file descriptor table.
void fd_table_detach(
    struct fd_table *ft,
    __wasi_fd_t fd,
    struct fd_object **fo
) REQUIRES_EXCLUSIVE(ft->lock) PRODUCES((*fo)->refcount) {
  assert(ft->size > fd && "File descriptor table too small");
  struct fd_entry *fe = &ft->entries[fd];
  *fo = fe->object;
  assert(*fo != NULL && "Attempted to detach nonexistent descriptor");
  fe->object = NULL;
  assert(ft->used > 0 && "Reference count mismatch");
  --ft->used;
}

// Looks up a file descriptor table entry by number and required rights.
__wasi_errno_t fd_table_get_entry(
    struct fd_table *ft,
    __wasi_fd_t fd,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    struct fd_entry **ret
) REQUIRES_SHARED(ft->lock) {
  // Test for file descriptor existence.
  if (fd >= ft->size)
    return __WASI_EBADF;
  struct fd_entry *fe = &ft->entries[fd];
  if (fe->object == NULL)
    return __WASI_EBADF;

  // Validate rights.
  if ((~fe->rights_base & rights_base) != 0 ||
      (~fe->rights_inheriting & rights_inheriting) != 0)
    return __WASI_ENOTCAPABLE;
  *ret = fe;
  return 0;
}

// Grows the file descriptor table to a required lower bound and a
// minimum number of free file descriptor table entries.
bool fd_table_grow(
    struct fd_table *ft,
    size_t min,
    size_t incr
) REQUIRES_EXCLUSIVE(ft->lock) {
  if (ft->size <= min || ft->size < (ft->used + incr) * 2) {
    // Keep on doubling the table size until we've met our constraints.
    size_t size = ft->size == 0 ? 1 : ft->size;
    while (size <= min || size < (ft->used + incr) * 2)
      size *= 2;

    // Grow the file descriptor table's allocation.
    struct fd_entry *entries = wasm_runtime_malloc((uint32)(sizeof(*entries) * size));
    if (entries == NULL)
      return false;

    if (ft->entries && ft->size > 0) {
      bh_memcpy_s(entries, (uint32)(sizeof(*entries) * size),
                  ft->entries, (uint32)(sizeof(*entries) * ft->size));
    }

    if (ft->entries)
      wasm_runtime_free(ft->entries);

    // Mark all new file descriptors as unused.
    for (size_t i = ft->size; i < size; ++i)
      entries[i].object = NULL;
    ft->entries = entries;
    ft->size = size;
  }
  return true;
}

// Inserts a file descriptor object into an unused slot of the file
// descriptor table.
__wasi_errno_t fd_table_insert(
    struct fd_table *ft,
    struct fd_object *fo,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    __wasi_fd_t *out
) REQUIRES_UNLOCKED(ft->lock) UNLOCKS(fo->refcount) {
  // Grow the file descriptor table if needed.
  rwlock_wrlock(&ft->lock);
  if (!fd_table_grow(ft, 0, 1)) {
    rwlock_unlock(&ft->lock);
    fd_object_release(fo);
    return convert_errno(errno);
  }

  *out = fd_table_unused(ft);
  fd_table_attach(ft, *out, fo, rights_base, rights_inheriting);
  rwlock_unlock(&ft->lock);
  return 0;
}

// Inserts an already existing file descriptor into the file descriptor
// table.
bool fd_table_insert_existing(
    struct fd_table *ft,
    __wasi_fd_t in,
    int out
) {
  __wasi_filetype_t type;
  __wasi_rights_t rights_base, rights_inheriting;
  if (fd_determine_type_rights(out, &type, &rights_base, &rights_inheriting) !=
      0)
    return false;

  struct fd_object *fo;
  __wasi_errno_t error = fd_object_new(type, &fo);
  if (error != 0)
    return false;
  fo->number = out;
  if (type == __WASI_FILETYPE_DIRECTORY) {
    mutex_init(&fo->directory.lock);
    fo->directory.handle = NULL;
  }

  // Grow the file descriptor table if needed.
  rwlock_wrlock(&ft->lock);
  if (!fd_table_grow(ft, in, 1)) {
    rwlock_unlock(&ft->lock);
    fd_object_release(fo);
    return false;
  }

  fd_table_attach(ft, in, fo, rights_base, rights_inheriting);
  rwlock_unlock(&ft->lock);
  return true;
}