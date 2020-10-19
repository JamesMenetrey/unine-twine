#ifndef WASI_API_FD_INTERNAL_H
#define WASI_API_FD_INTERNAL_H

#include <dirent.h>

#include "locking.h"
#include "refcount.h"

struct fd_entry;
struct fd_prestat;
struct syscalls;

struct fd_table {
  struct rwlock lock;
  struct fd_entry *entries;
  size_t size;
  size_t used;
};

struct fd_prestats {
  struct rwlock lock;
  struct fd_prestat *prestats;
  size_t size;
  size_t used;
};

struct fd_object {
  struct refcount refcount;
  __wasi_filetype_t type;
  int number;

  union {
    // Data associated with directory file descriptors.
    struct {
      struct mutex lock;            // Lock to protect members below.
      DIR *handle;                  // Directory handle.
      __wasi_dircookie_t offset;  // Offset of the directory.
    } directory;
  };
};

struct fd_entry {
  struct fd_object *object;
  __wasi_rights_t rights_base;
  __wasi_rights_t rights_inheriting;
};

// Lease to a directory, so a path underneath it can be accessed.
//
// This structure is used by system calls that operate on pathnames. In
// this environment, pathnames always consist of a pair of a file
// descriptor representing the directory where the lookup needs to start
// and the actual pathname string.
struct path_access {
  int fd;                       // Directory file descriptor.
  const char *path;             // Pathname.
  bool follow;                  // Whether symbolic links should be followed.
  char *path_start;             // Internal: pathname to free.
  struct fd_object *fd_object;  // Internal: directory file descriptor object.
};

#endif /* WASI_API_FD_INTERNAL_H */