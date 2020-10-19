#ifndef RANDOM_H
#define RANDOM_H

void prng_init(void);
unsigned int prng_get_next(void);
unsigned int prng_get_next_nonuniform_between(const unsigned int min, const unsigned int max);
unsigned int* prng_get_cached_slice(const int len);

#endif // RANDOM_H