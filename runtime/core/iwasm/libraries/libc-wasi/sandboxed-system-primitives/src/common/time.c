#include "common/time.h"

// Converts a CloudABI clock identifier to a POSIX clock identifier.
bool convert_clockid(
    __wasi_clockid_t in,
    clockid_t *out
) {
  switch (in) {
    case __WASI_CLOCK_MONOTONIC:
      *out = CLOCK_MONOTONIC;
      return true;
    case __WASI_CLOCK_PROCESS_CPUTIME_ID:
      *out = CLOCK_PROCESS_CPUTIME_ID;
      return true;
    case __WASI_CLOCK_REALTIME:
      *out = CLOCK_REALTIME;
      return true;
    case __WASI_CLOCK_THREAD_CPUTIME_ID:
      *out = CLOCK_THREAD_CPUTIME_ID;
      return true;
    default:
      return false;
  }
}

// Converts a POSIX timespec to a CloudABI timestamp.
__wasi_timestamp_t convert_timespec(
    const struct timespec *ts
) {
  if (ts->tv_sec < 0)
    return 0;
  if ((__wasi_timestamp_t)ts->tv_sec >= UINT64_MAX / 1000000000)
    return UINT64_MAX;
  return (__wasi_timestamp_t)ts->tv_sec * 1000000000 + (__wasi_timestamp_t)ts->tv_nsec;
}