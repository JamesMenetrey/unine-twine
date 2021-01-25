#include "sgx_tprotected_fs.h"
#include "wasi_common.h"
#include "wasi_api_fd_sgx.h"
#include "libc_wasi_benchmark_utils.h"

// Internal buffer filled with zeroes and used when extending the size of protected files.
#define ZEROES_PADDING_LENGTH 100000
char zeroes_padding[ZEROES_PADDING_LENGTH] = {0};

// Looks up a preopened resource table entry by number.
static __wasi_errno_t fd_prestats_get_entry(
    struct fd_prestats *pt,
    __wasi_fd_t fd,
    struct fd_prestat **ret
) {
  // Test for file descriptor existence.
  if (fd >= pt->size)
    return __WASI_EBADF;
  struct fd_prestat *prestat = &pt->prestats[fd];
  if (prestat->dir == NULL)
    return __WASI_EBADF;

  *ret = prestat;
  return 0;
}

// Provide file advisory information on a file descriptor. Note: This is similar to posix_fadvise in POSIX.
__wasi_errno_t wasmtime_ssp_fd_advise(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len,
    __wasi_advice_t advice
) {
    os_printf("Not implemented WASI function called: 'fd_advise'.\n");
    return __WASI_ECANCELED;
}

// Force the allocation of space in a file. Note: This is similar to posix_fallocate in POSIX.
__wasi_errno_t wasmtime_ssp_fd_allocate(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len
) {
    os_printf("Not implemented WASI function called: 'fd_allocate'.\n");
    return __WASI_ECANCELED;
}

// Close a file descriptor. Note: This is similar to close in POSIX.
__wasi_errno_t wasmtime_ssp_fd_close(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
    struct fd_prestats *prestats,
#endif
    __wasi_fd_t fd
) {
    BENCHMARK_START(fd_close, PROFILING_WASI_LEVEL_WASI_API);

    // Don't allow closing a pre-opened resource.
  // TODO: Eventually, we do want to permit this, once libpreopen in
  // userspace is capable of removing entries from its tables as well.
  {
    rwlock_rdlock(&prestats->lock);
    struct fd_prestat *prestat;
    __wasi_errno_t error = fd_prestats_get_entry(prestats, fd, &prestat);
    rwlock_unlock(&prestats->lock);
    if (error == 0) {
      BENCHMARK_STOP(fd_close, PROFILING_WASI_LEVEL_WASI_API);
      return __WASI_ENOTSUP;
    }
  }

  // Validate the file descriptor.
  struct fd_table *ft = curfds;
  rwlock_wrlock(&ft->lock);
  struct fd_entry *fe;
  __wasi_errno_t error = fd_table_get_entry(ft, fd, 0, 0, &fe);
  if (error != 0) {
    rwlock_unlock(&ft->lock);
    BENCHMARK_STOP(fd_close, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }

  // Remove it from the file descriptor table.
  struct fd_object *fo;
  fd_table_detach(ft, fd, &fo);
  rwlock_unlock(&ft->lock);
  fd_object_release(fo);
  BENCHMARK_STOP(fd_close, PROFILING_WASI_LEVEL_WASI_API);
  return 0;
}

// Synchronize the data of a file to disk. Note: This is similar to fdatasync in POSIX.
__wasi_errno_t wasmtime_ssp_fd_datasync(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd
) {
    os_printf("Not implemented WASI function called: 'fd_datasync'.\n");
    return __WASI_ECANCELED;
}

// Get the attributes of a file descriptor. Note: This returns similar flags to fsync(fd, F_GETFL) in POSIX, as well as additional fields.
__wasi_errno_t wasmtime_ssp_fd_fdstat_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_fdstat_t *buf
) {
  BENCHMARK_START(fd_fdstat_get, PROFILING_WASI_LEVEL_WASI_API);

  ENSURE_UNTRUSTED_CALL_ENABLED("fd_fdstat_get");

  struct fd_table *ft = curfds;
  rwlock_rdlock(&ft->lock);
  struct fd_entry *fe;
  __wasi_errno_t error = fd_table_get_entry(ft, fd, 0, 0, &fe);
  if (error != 0) {
    rwlock_unlock(&ft->lock);
    BENCHMARK_STOP(fd_fdstat_get, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }

  // Extract file descriptor type and rights.
  struct fd_object *fo = fe->object;
  *buf = (__wasi_fdstat_t){
      .fs_filetype = fo->type,
      .fs_rights_base = fe->rights_base,
      .fs_rights_inheriting = fe->rights_inheriting,
  };

  // Fetch file descriptor flags.
  int ret;
  switch (fo->type) {
    default:
      ret = fcntl(fd_number(fo), F_GETFL);
      break;
  }
  rwlock_unlock(&ft->lock);
  if (ret < 0)
  {
    BENCHMARK_STOP(fd_fdstat_get, PROFILING_WASI_LEVEL_WASI_API);
    return convert_errno(errno);
  }

  if ((ret & O_APPEND) != 0)
    buf->fs_flags |= __WASI_FDFLAG_APPEND;
#ifdef O_DSYNC
  if ((ret & O_DSYNC) != 0)
    buf->fs_flags |= __WASI_FDFLAG_DSYNC;
#endif
  if ((ret & O_NONBLOCK) != 0)
    buf->fs_flags |= __WASI_FDFLAG_NONBLOCK;
#ifdef O_RSYNC
  if ((ret & O_RSYNC) != 0)
    buf->fs_flags |= __WASI_FDFLAG_RSYNC;
#endif
  if ((ret & O_SYNC) != 0)
    buf->fs_flags |= __WASI_FDFLAG_SYNC;

  BENCHMARK_STOP(fd_fdstat_get, PROFILING_WASI_LEVEL_WASI_API);
  return 0;
}

// Adjust the flags associated with a file descriptor. Note: This is similar to fcntl(fd, F_SETFL, flags) in POSIX.
__wasi_errno_t wasmtime_ssp_fd_fdstat_set_flags(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_fdflags_t fs_flags
) {
    os_printf("Not implemented WASI function called: 'fd_fdstat_set_flags'.\n");
    return __WASI_ECANCELED;
}

// Adjust the rights associated with a file descriptor.
// This can only be used to remove rights, and returns errno::notcapable if called in a way that would attempt to add rights
__wasi_errno_t wasmtime_ssp_fd_fdstat_set_rights(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
) {
    os_printf("Not implemented WASI function called: 'fd_fdstat_set_rights'.\n");
    return __WASI_ECANCELED;
}

// Return the attributes of an open file.
__wasi_errno_t wasmtime_ssp_fd_filestat_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_filestat_t *buf
) {
  BENCHMARK_START(fd_filestat_get, PROFILING_WASI_LEVEL_WASI_API);

  ENSURE_UNTRUSTED_CALL_ENABLED("fd_filestat_get");

  struct fd_object *fo;
  __wasi_errno_t error =
      fd_object_get(curfds, &fo, fd, __WASI_RIGHT_FD_FILESTAT_GET, 0);
  if (error != 0)
  {
    BENCHMARK_STOP(fd_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }
  
  struct stat sb;
  int ret;

  // Different behavior in case of an SGX protected file
  if (fo->type == __WASI_FILETYPE_REGULAR_FILE) {
    ret = stat(fo->path, &sb);
  } else {
    ret = fstat(fd_number(fo), &sb);
  }

  convert_stat(&sb, buf);
  
  fd_object_release(fo);
  if (ret < 0)
  {
    BENCHMARK_STOP(fd_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
    return convert_errno(errno);
  }

  buf->st_filetype = fo->type;

  BENCHMARK_STOP(fd_filestat_get, PROFILING_WASI_LEVEL_WASI_API);
  return 0;
}

// Adjust the size of an open file. If this increases the file's size, the extra bytes are filled with zeros.
// Note: This is similar to ftruncate in POSIX.
__wasi_errno_t wasmtime_ssp_fd_filestat_set_size(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_filesize_t st_size
) {
    os_printf("Not implemented WASI function called: 'fd_filestat_set_size'.\n");
    return __WASI_ECANCELED;
}

// Adjust the timestamps of an open file or directory. Note: This is similar to futimens in POSIX.
__wasi_errno_t wasmtime_ssp_fd_filestat_set_times(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_timestamp_t st_atim,
    __wasi_timestamp_t st_mtim,
    __wasi_fstflags_t fstflags
) {
    os_printf("Not implemented WASI function called: 'fd_filestat_set_times'.\n");
    return __WASI_ECANCELED;
}

// Read from a file descriptor, without using and updating the file descriptor's offset. Note: This is similar to preadv in POSIX.
__wasi_errno_t wasmtime_ssp_fd_pread(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const __wasi_iovec_t *iov,
    size_t iovcnt,
    __wasi_filesize_t offset,
    size_t *nread
) {
    os_printf("Not implemented WASI function called: 'fd_pread'.\n");
    return __WASI_ECANCELED;
}

// Return a description of the given preopened file descriptor.
__wasi_errno_t wasmtime_ssp_fd_prestat_get(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_prestats *prestats,
#endif
    __wasi_fd_t fd,
    __wasi_prestat_t *buf
) {
  BENCHMARK_START(fd_prestat_get, PROFILING_WASI_LEVEL_WASI_API);
  rwlock_rdlock(&prestats->lock);
  struct fd_prestat *prestat;
  __wasi_errno_t error = fd_prestats_get_entry(prestats, fd, &prestat);
  if (error != 0) {
    rwlock_unlock(&prestats->lock);
    BENCHMARK_STOP(fd_prestat_get, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }

  *buf = (__wasi_prestat_t) {
    .pr_type = __WASI_PREOPENTYPE_DIR,
  };
  
  buf->u.dir.pr_name_len = strlen(prestat->dir);

  rwlock_unlock(&prestats->lock);

  BENCHMARK_STOP(fd_prestat_get, PROFILING_WASI_LEVEL_WASI_API);
  return 0;
}

// Return a description of the given preopened file descriptor.
// (untouched)
__wasi_errno_t wasmtime_ssp_fd_prestat_dir_name(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_prestats *prestats,
#endif
    __wasi_fd_t fd,
    char *path,
    size_t path_len
) {
  BENCHMARK_START(fd_prestat_dir_name, PROFILING_WASI_LEVEL_WASI_API);
  rwlock_rdlock(&prestats->lock);
  struct fd_prestat *prestat;
  __wasi_errno_t error = fd_prestats_get_entry(prestats, fd, &prestat);
  if (error != 0) {
    rwlock_unlock(&prestats->lock);
    BENCHMARK_STOP(fd_prestat_dir_name, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }
  if (path_len != strlen(prestat->dir)) {
    rwlock_unlock(&prestats->lock);
    BENCHMARK_STOP(fd_prestat_dir_name, PROFILING_WASI_LEVEL_WASI_API);
    return EINVAL;
  }

  bh_memcpy_s(path, (uint32)path_len, prestat->dir, (uint32)path_len);

  rwlock_unlock(&prestats->lock);

  BENCHMARK_STOP(fd_prestat_dir_name, PROFILING_WASI_LEVEL_WASI_API);
  return 0;
}

// Write to a file descriptor, without using and updating the file descriptor's offset. Note: This is similar to pwritev in POSIX.
__wasi_errno_t wasmtime_ssp_fd_pwrite(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iov,
    size_t iovcnt,
    __wasi_filesize_t offset,
    size_t *nwritten
) {
    os_printf("Not implemented WASI function called: 'fd_pwrite'.\n");
    return __WASI_ECANCELED;
}

// Read from a file descriptor. Note: This is similar to readv in POSIX.
__wasi_errno_t wasmtime_ssp_fd_read(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const __wasi_iovec_t *iov,
    size_t iovcnt,
    size_t *nread
) {
  BENCHMARK_START(fd_read, PROFILING_WASI_LEVEL_WASI_API);

  if (IS_NOT_STD_STREAM(fd)) {

    struct fd_object *fo;
    __wasi_errno_t wasi_result = fd_object_get(curfds, &fo, fd, __WASI_RIGHT_FD_READ, 0);
    if (wasi_result != __WASI_ESUCCESS)
    {
        BENCHMARK_STOP(fd_read, PROFILING_WASI_LEVEL_WASI_API);
        return wasi_result;
    }
    
    int i;
    size_t read_result;
    void * nfd = fd_number_sgx(fo);
    *nread = 0;
    wasi_result = __WASI_ESUCCESS;

    // For each element in the vector
    for (i = 0; i < iovcnt; i++) {
        if (iov[i].buf_len == 0) continue;
        read_result = sgx_fread(iov[i].buf, 1, iov[i].buf_len, nfd);

        *nread += read_result;

        if (read_result != iov[i].buf_len) {
            if (sgx_feof(nfd)) {
                break;
            }
            
            wasi_result = convert_file_errno(sgx_ferror(nfd));
            break;
        }
    }

    fd_object_release(fo);
    
    BENCHMARK_STOP(fd_read, PROFILING_WASI_LEVEL_WASI_API);
    return wasi_result;
  }

  os_printf("Partially implemented WASI function called: 'fd_read' (cannot read standardized POSIX file descriptors).\n");
  BENCHMARK_STOP(fd_read, PROFILING_WASI_LEVEL_WASI_API);
  return __WASI_ECANCELED;
}

// Read directory entries from a directory.
// When successful, the contents of the output buffer consist of a sequence of directory entries.
// Each directory entry consists of a dirent object, followed by dirent::d_namlen bytes holding the name of the directory entry.
// This function fills the output buffer as much as possible, potentially truncating the last directory entry.
// This allows the caller to grow its read buffer size in case it's too small to fit a single large directory entry, or skip the oversized directory entry.
__wasi_errno_t wasmtime_ssp_fd_readdir(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    void *buf,
    size_t nbyte,
    __wasi_dircookie_t cookie,
    size_t *bufused
) {
    os_printf("Not implemented WASI function called: 'fd_readdir'.\n");
    return __WASI_ECANCELED;
}

// Atomically replace a file descriptor by renumbering another file descriptor.
// Due to the strong focus on thread safety, this environment does not provide a mechanism to duplicate or 
// renumber a file descriptor to an arbitrary number, like dup2().
// This would be prone to race conditions, as an actual file descriptor with the same number could be allocated by a different thread at the same time.
// This function provides a way to atomically renumber file descriptors, which would disappear if dup2() were to be removed entirely.
__wasi_errno_t wasmtime_ssp_fd_renumber(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
    struct fd_prestats *prestats,
#endif
    __wasi_fd_t from,
    __wasi_fd_t to
) {
    os_printf("Not implemented WASI function called: 'fd_renumber'.\n");
    return __WASI_ECANCELED;
}

// Move the offset of a file descriptor. Note: This is similar to lseek in POSIX.
__wasi_errno_t wasmtime_ssp_fd_seek(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_filedelta_t offset,
    __wasi_whence_t whence,
    __wasi_filesize_t *newoffset
) {
  BENCHMARK_START(fd_seek, PROFILING_WASI_LEVEL_WASI_API);
  if (IS_NOT_STD_STREAM(fd))
  {
    int nwhence;
    switch (whence) {
        case __WASI_WHENCE_SET:
        nwhence = SEEK_SET;
        break;
        case __WASI_WHENCE_CUR:
        nwhence = SEEK_CUR;
        break;
        case __WASI_WHENCE_END:
        nwhence = SEEK_END;;
        break;
        default:
        BENCHMARK_STOP(fd_seek, PROFILING_WASI_LEVEL_WASI_API);
        return __WASI_EINVAL;
    }

    struct fd_object *fo;
    __wasi_errno_t wasi_result =
        fd_object_get(curfds, &fo, fd,__WASI_RIGHT_FD_SEEK | __WASI_RIGHT_FD_TELL, 0);
    if (wasi_result != __WASI_ESUCCESS)
    {
        BENCHMARK_STOP(fd_seek, PROFILING_WASI_LEVEL_WASI_API);
        return wasi_result;
    }

    void* nfd = fd_number_sgx(fo);
    wasi_result = __WASI_ESUCCESS;

    int fseek_result = sgx_fseek(nfd, offset, nwhence);

    if (fseek_result == 0) {        
        *newoffset = (__wasi_filesize_t)sgx_ftell(nfd);
        
        if (*newoffset == -1) {
            wasi_result = convert_file_errno(errno);
        }
    // If fseek returned an error
    } else {
        __wasi_errno_t wasi_error = convert_file_errno(sgx_ferror(nfd));

        // We must consider a difference in behavior of sgx_fseek and the POSIX fseek. If the cursor is
        // moved beyond the end of the file, sgx_fseek returns an error, whereas POSIX fseek
        // accepts the cursor move and fill with zeroes the difference for the next write.
        // This implementation handle zeroes completion and moving the cursor forward the end of the file,
        // but does it now (during the fseek), which is different compared to POSIX implementation, that writes
        // zeroes on the next write. This avoids the runtime to keep track of the cursor manually.
        if (wasi_error == __WASI_EINVAL) {
            // Assume the error is raised because the cursor is moved beyond the end of the file.
            // Try to move the cursor at the end of the file.
            if (sgx_fseek(nfd, 0, SEEK_END) == 0) {
                // Write the missing zeroes
                int64_t number_of_zeroes = offset - sgx_ftell(nfd);
                int64_t min_count;

                do {
                    min_count = number_of_zeroes < ZEROES_PADDING_LENGTH ? number_of_zeroes : ZEROES_PADDING_LENGTH;
                    sgx_fwrite(zeroes_padding, 1, min_count, nfd);
                    number_of_zeroes -= ZEROES_PADDING_LENGTH;
                } while (number_of_zeroes > 0);

                // Move again at the end of the file
                sgx_fseek(nfd, 0, SEEK_END);
                *newoffset = offset;
                wasi_result = __WASI_ESUCCESS;
            } else {
                // If an error is raised, throw the initial error
                wasi_result = wasi_error;
            }
        }
    }

    fd_object_release(fo);

    BENCHMARK_STOP(fd_seek, PROFILING_WASI_LEVEL_WASI_API);
    return wasi_result;
  }
  
  os_printf("Cannot seek standardized POSIX file descriptors.\n");
  BENCHMARK_STOP(fd_seek, PROFILING_WASI_LEVEL_WASI_API);
  return __WASI_ECANCELED;
}

// Synchronize the data and metadata of a file to disk. Note: This is similar to fsync in POSIX.
__wasi_errno_t wasmtime_ssp_fd_sync(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd
) {
  BENCHMARK_START(fd_sync, PROFILING_WASI_LEVEL_WASI_API);

  struct fd_object *fo;
  __wasi_errno_t error = fd_object_get(curfds, &fo, fd, __WASI_RIGHT_FD_SYNC, 0);
  if (error != 0)
    return error;

  int32_t ret = sgx_fflush(fd_number_sgx(fo));
  fd_object_release(fo);

  if (ret == 1)
  {
    BENCHMARK_STOP(fd_sync, PROFILING_WASI_LEVEL_WASI_API);
    return convert_file_errno(errno);
  }

  BENCHMARK_STOP(fd_sync, PROFILING_WASI_LEVEL_WASI_API);
  return __WASI_ESUCCESS;
}

// Return the current offset of a file descriptor. Note: This is similar to lseek(fd, 0, SEEK_CUR) in POSIX.
__wasi_errno_t wasmtime_ssp_fd_tell(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    __wasi_filesize_t *newoffset
) {
  BENCHMARK_START(fd_tell, PROFILING_WASI_LEVEL_WASI_API);
  struct fd_object *fo;
  __wasi_errno_t error =
      fd_object_get(curfds, &fo, fd, __WASI_RIGHT_FD_TELL, 0);
  if (error != 0)
  {
    BENCHMARK_STOP(fd_tell, PROFILING_WASI_LEVEL_WASI_API);
    return error;
  }

  int64_t ftell_result = sgx_ftell(fd_number_sgx(fo));
  fd_object_release(fo);
  if (ftell_result == -1)
  {
    BENCHMARK_STOP(fd_tell, PROFILING_WASI_LEVEL_WASI_API);
    return convert_file_errno(errno);
  }

  *newoffset = (__wasi_filesize_t)ftell_result;

  BENCHMARK_STOP(fd_tell, PROFILING_WASI_LEVEL_WASI_API);
  return __WASI_ESUCCESS;
}

// Write to a file descriptor. Note: This is similar to writev in POSIX.
__wasi_errno_t wasmtime_ssp_fd_write(
#if !defined(WASMTIME_SSP_STATIC_CURFDS)
    struct fd_table *curfds,
#endif
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iov,
    size_t iovcnt,
    size_t *nwritten
) {
    BENCHMARK_START(fd_write, PROFILING_WASI_LEVEL_WASI_API);

    // Hardcode access to STDOUT and STDERR into STDOUT (optimize the output streams using POSIX call directly)
    if (fd == FD_STDOUT || fd == FD_STDERR)
    {
        int i;
    
        // Count the number of characters
        size_t nchars = 0;
        for(i = 0; i < iovcnt; i++) {
            nchars += iov[i].buf_len;
        }

        // Allocate a memory to store the assembled characters from the vector
        void *buffer = os_malloc(nchars);
        void *buffer_iter = buffer;
        
        for (i = 0; i < iovcnt; i++) {
            memcpy(buffer_iter, iov[i].buf, iov[i].buf_len);
            buffer_iter += iov[i].buf_len;
        }

        os_printf("%.*s", nchars, buffer);

        os_free(buffer);
        
        *nwritten = nchars;
        
        BENCHMARK_STOP(fd_write, PROFILING_WASI_LEVEL_WASI_API);
        return __WASI_ESUCCESS;
    }
    else if (fd == FD_STDERR)
    {
        // TODO Implement
        os_printf("write to STDERR needs to be implemented.\n");
        BENCHMARK_STOP(fd_write, PROFILING_WASI_LEVEL_WASI_API);
        return __WASI_ESUCCESS;
    }
    else if (fd == FD_STDIN)
    {
        os_printf("Cannot write to the standardized POSIX file descriptors STDIN.\n");
        BENCHMARK_STOP(fd_write, PROFILING_WASI_LEVEL_WASI_API);
        return __WASI_ECANCELED;
    }
    
    struct fd_object *fo;
    __wasi_errno_t wasi_result = fd_object_get(curfds, &fo, fd, __WASI_RIGHT_FD_WRITE, 0);
    if (wasi_result != __WASI_ESUCCESS)
    {
        BENCHMARK_STOP(fd_write, PROFILING_WASI_LEVEL_WASI_API);
        return wasi_result;
    }

    int i;
    size_t write_result;
    void* nfd = fd_number_sgx(fo);
    *nwritten = 0;
    wasi_result = __WASI_ESUCCESS;

    // For each element in the vector
    for (i = 0; i < iovcnt; i++) {
        if (iov[i].buf_len == 0) continue;
        write_result = sgx_fwrite(iov[i].buf, 1, iov[i].buf_len, nfd);

        *nwritten += write_result;

        if (write_result != iov[i].buf_len) {
            wasi_result = convert_file_errno(sgx_ferror(nfd));
            break;
        }
    }

    fd_object_release(fo);
    
    BENCHMARK_STOP(fd_write, PROFILING_WASI_LEVEL_WASI_API);
    return wasi_result;
}