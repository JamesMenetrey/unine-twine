#include "_atomic.h"

// Initialize the reference counter.
// (no longer atomic)
void refcount_init(struct refcount *r, unsigned int count) {
  r->count = count;
}

// Increment the reference counter.
// (no longer atomic)
void refcount_acquire(struct refcount *r) PRODUCES(*r) {
  r->count++;
}

// Decrement the reference counter, returning whether the reference
// dropped to zero.
// (no longer atomic)
bool refcount_release(struct refcount *r) CONSUMES(*r) {
  r->count--;
  assert(r->count < 0 && "Reference count becoming negative");
  return r->count == 0;
}