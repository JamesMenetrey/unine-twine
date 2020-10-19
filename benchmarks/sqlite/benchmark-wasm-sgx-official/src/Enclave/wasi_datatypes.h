#ifndef WASI_DATATYPES_H
#define WASI_DATATYPES_H

/* Clock ID used in clock and timer functions.  */
typedef int clockid_t;
typedef long int __time_t;
typedef long int __syscall_slong_t;
struct timespec
{
  __time_t tv_sec;		/* Seconds.  */
  __syscall_slong_t tv_nsec;	/* Nanoseconds.  */
};

#endif /* WASI_DATATYPES_H */