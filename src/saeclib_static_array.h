#ifndef _SAECLIB_STATIC_ARRAY_H
#define _SAECLIB_STATIC_ARRAY_H

#include <stddef.h>

/**
 * Container for a resizable array with a fixed maximum capacity.
 *
 * Doesn't necessarily provide many useful functions, just a useful interface
 */


typedef struct saeclib_static_array
{
    // we use uint8_t as the base type for storing data as it's guaranteed to have sizeof() 1.
    uint8_t* data;

    // capacity is the total number of elements with size elt_size that can be held.
    size_t capacity;

    // current size of the array in number of elements.
    size_t size;

    // size in bytes of each element in the array
    size_t elt_size;
} saeclib_static_array_t;


#define saeclib_static_array_salloc(_capacity, _elt_size) \
    ({\
    saeclib_static_array_t ssa; \
    static uint8_t space[(_capacity) * (_elt_size)]; \
    ssa.data = space; \
    ssa.capacity = (_capacity); \
    ssa.size = 0; \
    ssa.elt_size = (_elt_size); \
    ssa; \
    })

#endif
