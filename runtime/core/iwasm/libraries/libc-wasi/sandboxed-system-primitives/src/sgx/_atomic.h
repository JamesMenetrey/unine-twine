#ifndef ATOMIC_H
#define ATOMIC_H

#include <stdbool.h>
#include "bh_assert.h"

#define LOCKABLE
#define CONSUMES(...)
#define PRODUCES(...)

typedef uint32_t atomic_uint;

// Simple reference counter.
// (untouched)
struct LOCKABLE refcount {
  atomic_uint count;
};

// Initialize the reference counter.
// (no longer atomic)
void refcount_init(struct refcount *r, unsigned int count);

// Increment the reference counter.
// (no longer atomic)
void refcount_acquire(struct refcount *r) PRODUCES(*r);

// Decrement the reference counter, returning whether the reference
// dropped to zero.
// (no longer atomic)
bool refcount_release(struct refcount *r) CONSUMES(*r);

#endif /* ATOMIC_H */