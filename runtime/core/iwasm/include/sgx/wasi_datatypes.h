#ifndef DATATYPES_H
#define DATATYPES_H

//
// Time
//

/* Identifier for system-wide realtime clock.  */
# define CLOCK_REALTIME			0
/* Monotonic system-wide clock.  */
# define CLOCK_MONOTONIC		1
/* High-resolution timer from the CPU.  */
# define CLOCK_PROCESS_CPUTIME_ID	2
/* Thread-specific CPU-time clock.  */
# define CLOCK_THREAD_CPUTIME_ID	3

/* Clock ID used in clock and timer functions.  */
typedef int clockid_t;
typedef long int __time_t;
typedef long int __syscall_slong_t;
struct timespec
{
  __time_t tv_sec;		/* Seconds.  */
  __syscall_slong_t tv_nsec;	/* Nanoseconds.  */
};

/// File system
#define F_DUPFD  0
#define F_GETFD  1
#define F_SETFD  2
#define F_GETFL  3
#define F_SETFL  4

typedef long int off_t;
typedef unsigned long dev_t;
typedef unsigned long ino_t;
typedef unsigned mode_t;
typedef unsigned long nlink_t;
typedef unsigned socklen_t;
typedef long blksize_t;
typedef long blkcnt_t;

typedef int pid_t;
typedef unsigned gid_t;
typedef unsigned uid_t;

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    nlink_t st_nlink;

    mode_t st_mode;
    uid_t st_uid;
    gid_t st_gid;
    unsigned int    __pad0;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;

    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
    long __unused[3];
};

#endif