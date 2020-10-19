#include "bh_common.h"
#include "wasi_internal.h"
#include "wasi_api_fd_sgx.h"


//
// Definition of missing functions
//

// As we are in an enclave, we do not have access to the file system.
// The resolved path will be a copy of the path.
// https://linux.die.net/man/3/realpath
char *realpath(
    const char *path,
    char *resolved_path)
{
    if (path == resolved_path) return resolved_path;

    b_strcpy_s(resolved_path, PATH_MAX, path);
    return resolved_path;
}

// As we are in an enclave, we do not want to pre-open directories.
// Return a blanket file descriptor.
// https://man7.org/linux/man-pages/man2/open.2.html
int open(
    const char *pathname,
    int flags,
    uint32_t mode)
{
    return 0;
}


//
// Reproduced functions from POSIX
//


// (untouched)
void fd_table_init(
    struct fd_table *ft
) {
  rwlock_init(&ft->lock);
  ft->entries = NULL;
  ft->size = 0;
  ft->used = 0;
#if defined(WASMTIME_SSP_STATIC_CURFDS)
  curfds = ft;
#endif
}

// (untouched)
void fd_prestats_init(
    struct fd_prestats *pt
) {
  rwlock_init(&pt->lock);
  pt->prestats = NULL;
  pt->size = 0;
  pt->used = 0;
#if defined(WASMTIME_SSP_STATIC_CURFDS)
  prestats = pt;
#endif
}

// Grows the preopened resource table to a required lower bound and a
// minimum number of free preopened resource table entries.
// (untouched)
static bool fd_prestats_grow(
    struct fd_prestats *pt,
    size_t min,
    size_t incr
) REQUIRES_EXCLUSIVE(pt->lock) {
  if (pt->size <= min || pt->size < (pt->used + incr) * 2) {
    // Keep on doubling the table size until we've met our constraints.
    size_t size = pt->size == 0 ? 1 : pt->size;
    while (size <= min || size < (pt->used + incr) * 2)
      size *= 2;

    // Grow the file descriptor table's allocation.
    struct fd_prestat *prestats = wasm_runtime_malloc((uint32)(sizeof(*prestats) * size));
    if (prestats == NULL)
      return false;

    if (pt->prestats && pt->size > 0) {
      bh_memcpy_s(prestats, (uint32)(sizeof(*prestats) * size),
                  pt->prestats, (uint32)(sizeof(*prestats) * pt->size));
    }

    if (pt->prestats)
      wasm_runtime_free(pt->prestats);

    // Mark all new file descriptors as unused.
    for (size_t i = pt->size; i < size; ++i)
      prestats[i].dir = NULL;
    pt->prestats = prestats;
    pt->size = size;
  }
  return true;
}

// Inserts a preopened resource record into the preopened resource table.
// (untouched)
bool fd_prestats_insert(
    struct fd_prestats *pt,
    const char *dir,
    __wasi_fd_t fd
) {
  // Grow the preopened resource table if needed.
  rwlock_wrlock(&pt->lock);
  if (!fd_prestats_grow(pt, fd, 1)) {
    rwlock_unlock(&pt->lock);
    return false;
  }

  pt->prestats[fd].dir = bh_strdup(dir);
  rwlock_unlock(&pt->lock);

  if (pt->prestats[fd].dir == NULL)
    return false;

  return true;
}

// (untouched)
bool argv_environ_init(struct argv_environ_values *argv_environ,
                       const size_t *argv_offsets, size_t argv_offsets_len,
                       const char *argv_buf, size_t argv_buf_len,
                       const size_t *environ_offsets, size_t environ_offsets_len,
                       const char *environ_buf, size_t environ_buf_len)
{
    uint64 total_size;
    size_t i;

    memset(argv_environ, 0, sizeof(struct argv_environ_values));

    argv_environ->argc = argv_offsets_len;
    argv_environ->argv_buf_size = argv_buf_len;

    total_size = sizeof(char *) * (uint64)argv_offsets_len;
    if (total_size >= UINT32_MAX
        || !(argv_environ->argv = wasm_runtime_malloc((uint32)total_size)))
        return false;


    if (argv_buf_len >= UINT32_MAX
        || !(argv_environ->argv_buf = wasm_runtime_malloc((uint32)argv_buf_len)))
        goto fail1;

    for (i = 0; i < argv_offsets_len; ++i) {
        argv_environ->argv[i] = argv_environ->argv_buf + argv_offsets[i];
    }
    bh_memcpy_s(argv_environ->argv_buf, (uint32)argv_buf_len,
                argv_buf, (uint32)argv_buf_len);

    argv_environ->environ_count = environ_offsets_len;
    argv_environ->environ_buf_size = environ_buf_len;

    total_size = sizeof(char *) * (uint64)environ_offsets_len;
    if (total_size >= UINT32_MAX
        || !(argv_environ->environ = wasm_runtime_malloc((uint32)total_size)))
        goto fail2;

    if (environ_buf_len >= UINT32_MAX
        || !(argv_environ->environ_buf = wasm_runtime_malloc((uint32)environ_buf_len)))
        goto fail3;

    for (i = 0; i < environ_offsets_len; ++i) {
        argv_environ->environ[i] = argv_environ->environ_buf + environ_offsets[i];
    }
    bh_memcpy_s(argv_environ->environ_buf, (uint32)environ_buf_len,
                environ_buf, (uint32)environ_buf_len);

    return true;

fail3:
    wasm_runtime_free(argv_environ->environ);
fail2:
    wasm_runtime_free(argv_environ->argv_buf);
fail1:
    wasm_runtime_free(argv_environ->argv);

    memset(argv_environ, 0, sizeof(struct argv_environ_values));
    return false;
}

// (untouched)
void argv_environ_destroy(struct argv_environ_values *argv_environ)
{
    if (argv_environ->argv_buf)
        wasm_runtime_free(argv_environ->argv_buf);
    if (argv_environ->argv)
        wasm_runtime_free(argv_environ->argv);
    if (argv_environ->environ_buf)
        wasm_runtime_free(argv_environ->environ_buf);
    if (argv_environ->environ)
        wasm_runtime_free(argv_environ->environ);
}

// (untouched)
void fd_table_destroy(struct fd_table *ft)
{
    if (ft->entries) {
        for (uint32 i = 0; i < ft->size; i++) {
            if (ft->entries[i].object != NULL) {
                fd_object_release(ft->entries[i].object);
            }
        }
        wasm_runtime_free(ft->entries);
    }
}

// (untouched)
void fd_prestats_destroy(struct fd_prestats *pt)
{
    if (pt->prestats) {
        for (uint32 i = 0; i < pt->size; i++) {
            if (pt->prestats[i].dir != NULL) {
                wasm_runtime_free((void*)pt->prestats[i].dir);
            }
        }
        wasm_runtime_free(pt->prestats);
    }
}
