#include "platform_api_vmcore.h"
#include "wasmtime_ssp.h"

// Converts a CloudABI clock identifier to a POSIX clock identifier.
bool convert_clockid(
    __wasi_clockid_t in,
    clockid_t *out
);

// Converts a POSIX timespec to a CloudABI timestamp.
__wasi_timestamp_t convert_timespec(
    const struct timespec *ts
);