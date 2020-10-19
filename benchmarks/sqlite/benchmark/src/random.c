#include <stdio.h>
#include "random.h"
#define INTERNAL_CACHE_SIZE 1024 * 1024
#define INTERNAL_CACHE_NUMBER_OF_ELEMENTS INTERNAL_CACHE_SIZE / sizeof(unsigned int)

static unsigned int internal_cache[INTERNAL_CACHE_SIZE];
static unsigned int x, y;

void prng_init(void)
{
    x = 0xad131d0b;
    y = 0x44f9eac8;

    int i;
    for (i = 0; i < INTERNAL_CACHE_NUMBER_OF_ELEMENTS; i++)
    {
        internal_cache[i] = prng_get_next();
    }
}

/*
    Return a pseudo-random unsigned integer.
    Source: https://www.sqlite.org/src/file?name=test/speedtest1.c
*/
unsigned int prng_get_next(void){
    x = (x>>1) ^ ((1+~(x&1)) & 0xd0000001);
    y = y*1103515245 + 12345;
    return x ^ y;
}

unsigned int prng_get_next_nonuniform_between(const unsigned int min, const unsigned int max)
{
    return prng_get_next() % (max - min) + min;
}

unsigned int* prng_get_cached_slice(const int len)
{
    if (len > INTERNAL_CACHE_SIZE)
    {
        return NULL;
    }

    return internal_cache + prng_get_next_nonuniform_between(0, INTERNAL_CACHE_NUMBER_OF_ELEMENTS - len - 1);
}