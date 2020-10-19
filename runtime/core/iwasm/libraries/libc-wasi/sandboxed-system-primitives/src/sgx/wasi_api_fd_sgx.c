#include "sgx_error.h"
#include "sgx_tprotected_fs.h"
#include "wasi_api_fd_sgx.h"
#include "common/str.h"
#include "common/rights.h"

// Converts a POSIX + SGX error code to a CloudABI error code.
__wasi_errno_t convert_file_errno(int error) {
  if (error < SGX_ERROR_FILE_BASE) {
    return convert_errno(error);
  }

  switch (error)
  {
  /* The file is in bad status */
  case SGX_ERROR_FILE_BAD_STATUS:
    return __WASI_EIO;
  /* The Key ID field is all zeros, can't re-generate the encryption key */
  case SGX_ERROR_FILE_NO_KEY_ID:
    return __WASI_EINVAL;
  /* The current file name is different then the original file name (not allowed, substitution attack) */
  case SGX_ERROR_FILE_NAME_MISMATCH:
    return __WASI_EINVAL;
  /* The file is not an SGX file */
  case SGX_ERROR_FILE_NOT_SGX_FILE:
    return __WASI_EIO;
  /* A recovery file can't be opened, so flush operation can't continue (only used when no EXXX is returned)  */
  case SGX_ERROR_FILE_CANT_OPEN_RECOVERY_FILE:
    return __WASI_EIO;
  /* A recovery file can't be written, so flush operation can't continue (only used when no EXXX is returned)  */
  case SGX_ERROR_FILE_CANT_WRITE_RECOVERY_FILE:
    return __WASI_EIO;
  /* When openeing the file, recovery is needed, but the recovery process failed */
  case SGX_ERROR_FILE_RECOVERY_NEEDED:
    return __WASI_EIO;
  /* fflush operation (to disk) failed (only used when no EXXX is returned) */
  case SGX_ERROR_FILE_FLUSH_FAILED:
    return __WASI_EIO;
  /* fclose operation (to disk) failed (only used when no EXXX is returned) */
  case SGX_ERROR_FILE_CLOSE_FAILED:
    return __WASI_EIO;
  }
  os_printf(" > unconverted error: %d\n", error);
  return __WASI_ENOSYS;
}

// Lowers the reference count on a file descriptor object. When the
// reference count reaches zero, its resources are cleaned up.
void fd_object_release(
    struct fd_object *fo
) UNLOCKS(fo->refcount) {
  if (refcount_release(&fo->refcount)) {
    switch (fo->type) {
      case __WASI_FILETYPE_REGULAR_FILE:
        if(IS_NOT_STD_STREAM(fd_number(fo))) {
          sgx_fclose(fd_number_sgx(fo));
        }
        break;
      default:
        // TODO Not handled. Would we need this ?
        break;
    }
    wasm_runtime_free(fo);
  }
}

void path_put(
    struct path_access *pa
) UNLOCKS(pa->fd_object->refcount) {
  wasm_runtime_free(pa->path_start);

  if (fd_number(pa->fd_object) != pa->fd)
  {
    // TODO Implement a proper sgx fclose (check posix.c and understand what it does)
    os_printf("Not implemented\n");
    // sgx_fclose(pa->fd);
  }

  fd_object_release(pa->fd_object);
}

// Determines the type of a file descriptor and its maximum set of
// rights that should be attached to it.
__wasi_errno_t fd_determine_type_rights(
    int fd,
    __wasi_filetype_t *type,
    __wasi_rights_t *rights_base,
    __wasi_rights_t *rights_inheriting
) {  
  struct stat sb;
  if (fstat(fd, &sb) < 0)
    return convert_errno(errno);
  if (S_ISBLK(sb.st_mode)) {
    *type = __WASI_FILETYPE_BLOCK_DEVICE;
    *rights_base = RIGHTS_BLOCK_DEVICE_BASE;
    *rights_inheriting = RIGHTS_BLOCK_DEVICE_INHERITING;
  } else if (S_ISCHR(sb.st_mode)) {
    *type = __WASI_FILETYPE_CHARACTER_DEVICE;
    *rights_base = RIGHTS_CHARACTER_DEVICE_BASE;
    *rights_inheriting = RIGHTS_CHARACTER_DEVICE_INHERITING;
  } else if (S_ISDIR(sb.st_mode)) {
    *type = __WASI_FILETYPE_DIRECTORY;
    *rights_base = RIGHTS_DIRECTORY_BASE;
    *rights_inheriting = RIGHTS_DIRECTORY_INHERITING;
  } else if (S_ISREG(sb.st_mode)) {
    *type = __WASI_FILETYPE_REGULAR_FILE;
    *rights_base = RIGHTS_REGULAR_FILE_BASE;
    *rights_inheriting = RIGHTS_REGULAR_FILE_INHERITING;
  } else if (S_ISFIFO(sb.st_mode)) {
    *type = __WASI_FILETYPE_SOCKET_STREAM;
    *rights_base = RIGHTS_SOCKET_BASE;
    *rights_inheriting = RIGHTS_SOCKET_INHERITING;
  } else {
    return __WASI_EINVAL;
  }

  // Strip off read/write bits based on the access mode.
  switch (fcntl(fd, F_GETFL) & O_ACCMODE) {
    case O_RDONLY:
      *rights_base &= ~(__wasi_rights_t)__WASI_RIGHT_FD_WRITE;
      break;
    case O_WRONLY:
      *rights_base &= ~(__wasi_rights_t)__WASI_RIGHT_FD_READ;
      break;
  }
  return 0;
}

// Inserts a numerical file descriptor into the file descriptor table.
// (TODO Should be removed for in favor of fd_table_insert_fd_sgx ?)
__wasi_errno_t fd_table_insert_fd(
    struct fd_table *ft,
    int in,
    __wasi_filetype_t type,
    __wasi_rights_t rights_base,
    __wasi_rights_t rights_inheriting,
    __wasi_fd_t *out
) REQUIRES_UNLOCKED(ft->lock) {
  struct fd_object *fo;
  __wasi_errno_t error = fd_object_new(type, &fo);
  if (error != 0) {
    //sgx_fclose(in);
    // TODO sgx fclose does not apply to int
    return error;
  }
  fo->number = in;
  if (type == __WASI_FILETYPE_DIRECTORY) {
    mutex_init(&fo->directory.lock);
    fo->directory.handle = NULL;
  }
  
  return fd_table_insert(ft, fo, rights_base, rights_inheriting, out);
}

// Picks an unused slot from the file descriptor table.
// (modified to remove the random part)
__wasi_fd_t fd_table_unused(
    struct fd_table *ft
) REQUIRES_SHARED(ft->lock) {
  assert(ft->size > ft->used && "File descriptor table has no free slots");
  uint i;
  for (i = 0; i < ft->size; i++) {
    __wasi_fd_t fd = (__wasi_fd_t)i;
    if (ft->entries[fd].object == NULL)
      return fd;
  }

  assert("File description table starved");
  return -1;
}

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
) TRYLOCKS_EXCLUSIVE(0, pa->fd_object->refcount) {

  char *path = str_nullterminate(upath, upathlen);
  if (path == NULL)
    return convert_errno(errno);

  // Fetch the directory file descriptor.
  struct fd_object *fo;
  __wasi_errno_t error =
      fd_object_get(curfds, &fo, fd, rights_base, rights_inheriting);
  if (error != 0) {
    wasm_runtime_free(path);
    return error;
  }

  // TODO Check the rights based on the directory

  // Fill the path access
  pa->fd = fd_number(fo);
  pa->path = pa->path_start = path;
  pa->follow = (flags & __WASI_LOOKUP_SYMLINK_FOLLOW) != 0;
  pa->fd_object = fo;
  
  return __WASI_ESUCCESS;
}


// Returns the underlying SGX file descriptor number of a file descriptor
// object. This function can only be applied to objects that have an
// underlying file descriptor number.
void* fd_number_sgx(
    const struct fd_object *fo
) {
  return fo->sgx_file;
}

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
) REQUIRES_UNLOCKED(ft->lock) {
  struct fd_object *fo;
  __wasi_errno_t error = fd_object_new(type, &fo);
  if (error != 0) {
    sgx_fclose(in);
    return error;
  }
  fo->sgx_file = in;
  if (type == __WASI_FILETYPE_DIRECTORY) {
    mutex_init(&fo->directory.lock);
    fo->directory.handle = NULL;
  }

  bh_strcpy_s((char*)fo->path, PATH_LEN_MAX, path);
  fo->pathlen = pathlen;
  
  return fd_table_insert(ft, fo, rights_base, rights_inheriting, out);
}