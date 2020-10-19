#ifndef LOCKING_H
#define LOCKING_H


struct mutex {};
struct rwlock {}; // Define an empty struct for locking, since it must be SGX specific.

//
// Definition of missing functions
//

inline void mutex_init(
    struct mutex *lock
) {
  // No nothing. Need to be implemented in a SGX fashion.
}

inline void mutex_destroy(
    struct mutex *lock
) {
  // No nothing. Need to be implemented in a SGX fashion.
}

inline void rwlock_init(
    struct rwlock *lock
) {
    // No nothing. Need to be implemented in a SGX fashion.
}

inline void rwlock_wrlock(
    struct rwlock *lock
) {
  // No nothing. Need to be implemented in a SGX fashion.
}

inline void rwlock_unlock(
    struct rwlock *lock
) {
    // No nothing. Need to be implemented in a SGX fashion.
}

inline void rwlock_rdlock(
    struct rwlock *lock
) {
  // No nothing. Need to be implemented in a SGX fashion.
}

#endif /* LOCKING_H */