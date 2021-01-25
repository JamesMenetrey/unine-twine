#include "sgx_tprotected_fs.h"
#include "wasi_common.h"
#include "wasi_api_fd_sgx.h"
#include "common/rights.h"
#include "libc_wasi_benchmark_utils.h"

// Create a directory. Note: This is similar to mkdirat in POSIX.
__wasi_errno_t wasmtime_ssp_path_create_directory(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const char *path,
    size_t pathlen
) {
    os_printf("Not implemented WASI function called: 'path_create_directory'.\n");
    return __WASI_ECANCELED;
}

// Return the attributes of a file or directory. Note: This is similar to stat in POSIX.
__wasi_errno_t wasmtime_ssp_path_filestat_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *path,
    size_t pathlen,
    __wasi_filestat_t *buf
) {
  BENCHMARK_START(path_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
  struct path_access pa;
  __wasi_errno_t error =
      path_get(curfds, &pa, fd, flags, path, pathlen, __WASI_RIGHT_PATH_FILESTAT_GET, 0, false);
  if (error != 0)
  {
    BENCHMARK_STOP(path_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }

  struct stat sb;
  int ret = stat(pa.path, &sb);
  path_put(&pa);
  if (ret < 0){
    BENCHMARK_STOP(path_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
    return convert_errno(errno);
  }
  convert_stat(&sb, buf);

  // Convert the file type. In the case of sockets there is no way we
  // can easily determine the exact socket type.
  if (S_ISBLK(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_BLOCK_DEVICE;
  else if (S_ISCHR(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_CHARACTER_DEVICE;
  else if (S_ISDIR(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_DIRECTORY;
  else if (S_ISFIFO(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_SOCKET_STREAM;
  else if (S_ISLNK(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_SYMBOLIC_LINK;
  else if (S_ISREG(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_REGULAR_FILE;
  else if (S_ISSOCK(sb.st_mode))
    buf->st_filetype = __WASI_FILETYPE_SOCKET_STREAM;

  BENCHMARK_STOP(path_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
  return 0;
}

// Adjust the timestamps of a file or directory. Note: This is similar to utimensat in POSIX.
__wasi_errno_t wasmtime_ssp_path_filestat_set_times(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *path,
    size_t pathlen,
    __wasi_timestamp_t st_atim,
    __wasi_timestamp_t st_mtim,
    __wasi_fstflags_t fstflags
) {
    os_printf("Not implemented WASI function called: 'path_filestat_set_times'.\n");
    return __WASI_ECANCELED;
}

// Create a hard link. Note: This is similar to linkat in POSIX.
__wasi_errno_t wasmtime_ssp_path_link(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
    struct fd_prestats *prestats,
#endif
    __wasi_fd_t old_fd,
    __wasi_lookupflags_t old_flags,
    const char *old_path,
    size_t old_path_len,
    __wasi_fd_t new_fd,
    const char *new_path,
    size_t new_path_len
) {
    os_printf("Not implemented WASI function called: 'path_link'.\n");
    return __WASI_ECANCELED;
}

// Open a file or directory. The returned file descriptor is not guaranteed to be the lowest-numbered file descriptor not currently open;
// it is randomized to prevent applications from depending on making assumptions about indexes, since this is error-prone in multi-threaded contexts.
// The returned file descriptor is guaranteed to be less than 2**31. Note: This is similar to openat in POSIX.
//
// The implementation of this function relies on the Intel Protected File System Library. Therefore, only files can be opened.
__wasi_errno_t wasmtime_ssp_path_open(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t dirfd,
    __wasi_lookupflags_t dirflags,
    const char *path,
    size_t pathlen,
    __wasi_oflags_t oflags,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting,
    __wasi_fdflags_t fs_flags,
    __wasi_fd_t *fd
) {
  BENCHMARK_START(path_open, PROFILING_WASI_LEVEL_WASI_API);
  // TODO Refine the rights, based on the POSIX implem.
  const char *mode;

  bool read = (fs_rights_base & __WASI_RIGHT_FD_READ) != 0;
  bool write = (fs_rights_base & __WASI_RIGHT_FD_WRITE) != 0;
  bool must_create = (oflags & __WASI_O_CREAT) != 0;
  bool must_truncate = (oflags & __WASI_O_TRUNC) != 0;
  bool must_append = (fs_flags & __WASI_FDFLAG_APPEND) != 0;
  bool read_only = (read & !write) != 0;
  bool write_only = (!read & write) != 0;
  bool read_write = (read & write) != 0;

  // The mapping of the mode are described in the table in the official specifications:
  // https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
  if      (read_only) mode = "r";
  else if (write_only && must_create && must_truncate) mode = "w";
  else if (write_only && must_create && must_append) mode = "a";
  else if (read_write && must_create && must_truncate) mode = "w+";
  else if (read_write && must_create && must_append) mode = "a+";
  else if (read_write && must_create) {
      // No open mode for that use case. Simulate it.
      struct stat stat_st;
      int file_exists = stat(path, &stat_st) == 0;
      if (file_exists) mode = "r+";
      else mode = "w+";
  }
  else if (read_write) mode = "r+";
  else mode = NULL;
  
  // Cannot map the requested access to the SGX PFS
  if (mode == NULL) {
      BENCHMARK_STOP(path_open, PROFILING_WASI_LEVEL_WASI_API);
      return __WASI_ENOTCAPABLE;
  }
  
  // Which rights are needed on the directory file descriptor.
  __wasi_rights_t needed_base = __WASI_RIGHT_PATH_OPEN;
  __wasi_rights_t needed_inheriting = fs_rights_base | fs_rights_inheriting;

  if (must_create) needed_base |= __WASI_RIGHT_PATH_CREATE_FILE;
  if (must_truncate) needed_base |= __WASI_RIGHT_PATH_FILESTAT_SET_SIZE;

  struct path_access pa = { 0 };
  __wasi_errno_t error =
      path_get(curfds, &pa, dirfd, dirflags, path, pathlen, needed_base, needed_inheriting,
              (oflags & __WASI_O_CREAT) != 0);
  if (error != 0) {
      BENCHMARK_STOP(path_open, PROFILING_WASI_LEVEL_WASI_API);
      return error;
  }

  void* nfd = sgx_fopen_auto_key(pa.path, mode);
  if (nfd == NULL) {
      BENCHMARK_STOP(path_open, PROFILING_WASI_LEVEL_WASI_API);
      return convert_file_errno(errno);
  }

  path_put(&pa);

  // Assume a file is opened.
  __wasi_filetype_t type = __WASI_FILETYPE_REGULAR_FILE;

  BENCHMARK_STOP(path_open, PROFILING_WASI_LEVEL_WASI_API);
  return fd_table_insert_fd_sgx(curfds, nfd, type, RIGHTS_REGULAR_FILE_BASE, RIGHTS_REGULAR_FILE_BASE, fd, path, pathlen);
}

// Read the contents of a symbolic link. Note: This is similar to readlinkat in POSIX.
__wasi_errno_t wasmtime_ssp_path_readlink(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const char *path,
    size_t pathlen,
    char *buf,
    size_t bufsize,
    size_t *bufused
) {
    os_printf("Not implemented WASI function called: 'path_readlink'.\n");
    return __WASI_ECANCELED;
}

// Remove a directory. Return errno::notempty if the directory is not empty. Note: This is similar to unlinkat(fd, path, AT_REMOVEDIR) in POSIX.
__wasi_errno_t wasmtime_ssp_path_remove_directory(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const char *path,
    size_t pathlen
) {
    os_printf("Not implemented WASI function called: 'path_remove_directory'.\n");
    return __WASI_ECANCELED;
}

// Rename a file or directory. Note: This is similar to renameat in POSIX.
__wasi_errno_t wasmtime_ssp_path_rename(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t old_fd,
    const char *old_path,
    size_t old_path_len,
    __wasi_fd_t new_fd,
    const char *new_path,
    size_t new_path_len
) {
    os_printf("Not implemented WASI function called: 'path_rename'.\n");
    return __WASI_ECANCELED;
}

// Create a symbolic link. Note: This is similar to symlinkat in POSIX.
__wasi_errno_t wasmtime_ssp_path_symlink(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
    struct fd_prestats *prestats,
#endif
    const char *old_path,
    size_t old_path_len,
    __wasi_fd_t fd,
    const char *new_path,
    size_t new_path_len
) {
    os_printf("Not implemented WASI function called: 'path_symlink'.\n");
    return __WASI_ECANCELED;
}

// Unlink a file. Return errno::isdir if the path refers to a directory. Note: This is similar to unlinkat(fd, path, 0) in POSIX.
__wasi_errno_t wasmtime_ssp_path_unlink_file(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const char *path,
    size_t pathlen
) {
    if(sgx_remove(path) != 0)
    {
        return convert_file_errno(errno);
    }

    return __WASI_ESUCCESS;
}