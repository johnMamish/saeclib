#ifndef _SAECLIB_CIRCULAR_BUFFER_H
#define _SAECLIB_CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "saeclib_circular_buffer.h"
#include "saeclib_error.h"

typedef struct saeclib_collection
{
    // we use uint8_t as the base type for storing data as it's guaranteed to have sizeof() 1.
    uint8_t* data;

    // This queue is used for keeping track of open slots.
    saeclib_circular_buffer_t slots;

    // This bitmap is a fast way to determine which slots are occupied when iterating.
    uint32_t* occupied_bitmap;
} saeclib_collection_t;


typedef struct saeclib_collection_iterator
{
    int idx;
} saeclib_collection_iterator_t;

saeclib_error_e saeclib_collection_init(saeclib_collection_t* collection,
                                        void* bufspace,
                                        size_t bufsize,
                                        size_t eltsize,
                                        saeclib_circular_buffer_t* slots,
                                        uint32_t* bitmap_space);

#define saeclib_collection_salloc(capacity, elt_size) \
    ({ \
    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc((capacity), sizeof(void*)); \
    static uint32_t bitmap_space[(capacity / 32) + 1]; \
    saeclib_collection_t scl; \
    static uint8_t space[(capacity) * (elt_size)]; \
    saeclib_collection_init(&scl, space, (capacity) * (elt_size), (elt_size), &scb, bitmap_space); \
    scl; \
    })


saeclib_error_e saeclib_collection_add(saeclib_collection_t* collection,
                                       void* item);

saeclib_error_e saeclib_collection_iterator_init(saeclib_collection_t* collection,
                                                 saeclib_collection_iterator* it);

saeclib_error_e saeclib_collection_iterator_next(saeclib_collection_t* collection,
                                                 saeclib_collection_iterator* it);



#endif
