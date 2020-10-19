#ifndef WASI_API_FD_INTERNAL_H
#define WASI_API_FD_INTERNAL_H
#include "wasmtime_ssp.h"
#include "_atomic.h"
#include "_locking.h"



// POSIX specific
#define PATH_MAX 4096
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define O_CREAT        0100
#define O_EXCL         0200
#define O_NOCTTY       0400
#define O_TRUNC       01000
#define O_APPEND      02000
#define O_NONBLOCK    04000
#define O_DSYNC      010000
#define O_SYNC     04010000
#define O_RSYNC    04010000
#define O_DIRECTORY 0200000
#define O_NOFOLLOW  0400000
#define O_CLOEXEC  02000000

#define S_IFMT  0170000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFBLK 0060000
#define S_IFREG 0100000
#define S_IFIFO 0010000
#define S_IFLNK 0120000
#define S_IFSOCK 0140000

#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR 0x200
#define AT_SYMLINK_FOLLOW 0x400

#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#define S_ISCHR(mode)  (((mode) & S_IFMT) == S_IFCHR)
#define S_ISBLK(mode)  (((mode) & S_IFMT) == S_IFBLK)
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#define S_ISLNK(mode)  (((mode) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)

// Redefine the symbols related to the file system
#define O_PATH    010000000
#define O_SEARCH  O_PATH
#define O_EXEC    O_PATH

#define O_ACCMODE (03|O_SEARCH)
#define O_RDONLY  00
#define O_WRONLY  01
#define O_RDWR    02

#define O_DIRECTORY 0200000
char *realpath(const char *path, char *resolved_path);
int open(const char *pathname, int flags, uint32_t mode);

//
// Definition of missing macros
//
#define NO_LOCK_ANALYSIS 
#define UNLOCKS(...) 
#define TRYLOCKS_SHARED(...)
#define REQUIRES_EXCLUSIVE(...)
#define TRYLOCKS_EXCLUSIVE(...)
#define REQUIRES_SHARED(...)
#define REQUIRES_UNLOCKED(...)


typedef int32_t handle;
typedef handle DIR;


struct fd_entry;
struct fd_prestat;
struct syscalls;

//
// Reproduced structures from POSIX
//

// (untouched)
struct fd_entry {
  struct fd_object *object;
  __wasi_rights_t rights_base;
  __wasi_rights_t rights_inheriting;
};

#define PATH_LEN_MAX 255

// (modified to add sgx_file and path relation information)
struct fd_object {
  struct refcount refcount;
  __wasi_filetype_t type;

  // Contains either a file descriptor for POSIX and for SGX protected files
  int number;
  void* sgx_file;

  union {
    // Data associated with directory file descriptors.
    struct {
      struct mutex lock;            // Lock to protect members below.
      DIR *handle;                  // Directory handle.
      __wasi_dircookie_t offset;  // Offset of the directory.
    } directory;
  };
  
  const char path[PATH_LEN_MAX]; // Added for SGX PFS
  size_t pathlen; // Added for SGX PFS
};

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

// (untouched)
struct fd_prestat {
  const char *dir;
};



// Lease to a directory, so a path underneath it can be accessed.
//
// This structure is used by system calls that operate on pathnames. In
// this environment, pathnames always consist of a pair of a file
// descriptor representing the directory where the lookup needs to start
// and the actual pathname string.
// (untouched)
struct path_access {
  int fd;                       // Directory file descriptor.
  const char *path;             // Pathname.
  bool follow;                  // Whether symbolic links should be followed.
  char *path_start;             // Internal: pathname to free.
  struct fd_object *fd_object;  // Internal: directory file descriptor object.
};

#endif /* WASI_API_FD_INTERNAL_H */