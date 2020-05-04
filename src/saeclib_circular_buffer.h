#ifndef _SAECLIB_CIRCULAR_BUFFER_H
#define _SAECLIB_CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "saeclib_error.h"

typedef struct saeclib_circular_buffer
{
    // we use uint8_t as the base type for storing data as it's guaranteed to have sizeof() 1.
    uint8_t* data;

    // head and tail of the circular buffer. These details should be irrelevant, but in case a user
    // really wants to muck around in the guts, data is added at head and removed at tail.
    int head, tail;

    // Total number of elements that the buffer can hold. Data should have capacity * elt_size bytes
    // alloated to it.
    size_t capacity;
    size_t elt_size;
} saeclib_circular_buffer_t;

/**
 * Initializes a saeclib circular buffer structure with the given parameters.
 *
 * @param[in,out] buf         The buffer that should be initialized.
 * @param[in]     bufspace    Pointer to a statically allocated memory region containing enough
 *                            memory to satisfy the circular buffer: capacity * elt_size bytes.
 *                            For the sake of implementation simplicity, the buffer can only store
 *                            capacity - 1 elements. The extra code just isn't worth the one extra
 *                            element IMO.
 * @param[in]     bufsize     Size of bufspace in bytes. sizeof(bufspace) should be passed in.
 * @param[in]     elt_size    Size of elements to be stored in the buffer. If you give this
 *                            function a chunk of memory that's not a multiple of elt_size, it
 *                            won't complain, but memory will be wasted.
 *
 * @returns This function should always return SAECLIB_ERROR_NOERROR. If not enough memory is
 *          provided to store any elements, well, then, you get a circular buffer of size 0.
 */
saeclib_error_e saeclib_circular_buffer_init(saeclib_circular_buffer_t* buf,
                                               void* bufspace,
                                               size_t bufsize,
                                               size_t eltsize);

/**
 * As with all salloc macros in saeclib, use with caution. Containers initialized with a salloc
 * macro must not be passed out of a function that's called more than once.
 *
 * Statically allocates an appropriate amount of memory in an inner scope and then calls
 * saeclib_circular_buffer_init on it.
 *
 * @param[in]     capacity    How many elements should the circular buffer be able to hold?
 * @param[in]     elt_size    What's the size of each element, in bytes?
 *
 * @return This macro passes out a saeclib_circular_buffer_t.
 *
 * Example usage:
 *
 *     // create a circular buffer that can hold 16 mystruct_t's.
 *     saeclib_circular_buffer_t cb = saeclib_circular_buffer_salloc(16, sizeof(mystruct_t));
 */
#define saeclib_circular_buffer_salloc(capacity, elt_size) \
    ({ \
        saeclib_circular_buffer_t scb; \
        static uint8_t space[(capacity) * (elt_size)]; \
        saeclib_circular_buffer_init(&scb, space, (capacity) * (elt_size), (elt_size)); \
        scb; \
    })

/**
 * Returns the capacity of the circular buffer
 */
size_t saeclib_circular_buffer_capacity(const saeclib_circular_buffer_t* buf);

/**
 * Returns the number of elements currently in the circular buffer
 */
size_t saeclib_circular_buffer_size(const saeclib_circular_buffer_t* buf);

/**
 * Returns true if the circular buffer is empty.
 */
bool saeclib_circular_buffer_empty(const saeclib_circular_buffer_t* buf);

/**
 * Inserts a new item into the buffer at its head.
 *
 * @param[in,out] buf         The buffer to which the new item should be pushed
 * @param[in]     item        The item to push
 *
 * @returns normally SAECLIB_ERROR_NOERROR.
 *          If the buffer has no space left, the buffer is unchanged and
 *          saeclib_cicular_buffer_pushone() returns SAECLIB_ERROR_OVERFLOW.
 */
saeclib_error_e saeclib_circular_buffer_pushone(saeclib_circular_buffer_t* buf,
                                                const void* item);

/**
 * Inserts several new items into the buffer at its head.
 *
 * @param[in,out] buf         The buffer to which the new item should be pushed
 * @param[in]     items       Pointer to memory region containing the items to push
 * @param[in]     numel       The number of elements to insert into the buffer (not number of
 *                            bytes).
 *
 * @returns normally SAECLIB_ERROR_NOERROR.
 *          If the buffer has no space left, the buffer is unchanged and
 *          saeclib_cicular_buffer_pushmany() returns SAECLIB_ERROR_OVERFLOW.
 */
saeclib_error_e saeclib_circular_buffer_pushmany(saeclib_circular_buffer_t* buf,
                                                 const void* items,
                                                 uint32_t numel);

/**
 * Removes a single item from the buffer at its tail.
 *
 * @param[in,out] buf         The buffer from which an item should be gotten
 * @param[out]    item        Pointer to a memory location to which the fetched item should be
 *                            written. Should point to a region large enough to hold one element.
 *
 * @returns normally SAECLIB_ERROR_NOERROR.
 *          If the buffer has no elements in it, the buffer is unchanged and
 *          saeclib_cicular_buffer_popone() returns SAECLIB_ERROR_UNDERFLOW.
 */
saeclib_error_e saeclib_circular_buffer_popone(saeclib_circular_buffer_t* buf,
                                               void* item);

/**
 * Removes several items from the buffer at its tail.
 *
 * @param[in,out] buf         The buffer from which an item should be gotten
 * @param[out]    items       Pointer to a memory location to which the fetched item should be
 *                            written. Should point to a region large enough to hold numel elements.
 * @param[in]     numel       The number of elements to remove from the buffer (number of elements,
 *                            not the number of bytes).
 *
 * @returns normally SAECLIB_ERROR_NOERROR.
 *          If the buffer has too few elements in it, the buffer is unchanged and
 *          saeclib_cicular_buffer_popmany() returns SAECLIB_ERROR_UNDERFLOW.
 */
saeclib_error_e saeclib_circular_buffer_popmany(saeclib_circular_buffer_t* buf,
                                                void* items,
                                                uint32_t numel);

/**
 * Copies the element at the buffer's tail into a target memory region without removing it from the
 * buffer.
 *
 * @param[in]     buf         The buffer whose topmost element should be peeked at
 * @param[out]    item        Pointer to a memory location where we can put the element that's
 *                            being peeked at.
 *
 * @returns normally SAECLIB_ERROR_NOERROR.
 *          If the buffer has too few elements in it, *item is unchanged and
 *          SAECLIB_ERROR_UNDERFLOW is returned
 */
saeclib_error_e saeclib_circular_buffer_peekone(const saeclib_circular_buffer_t* buf,
                                                void* item);

/**
 *
 */
saeclib_error_e saeclib_circular_buffer_peekmany(const saeclib_circular_buffer_t* buf,
                                                 void* item,
                                                 uint32_t numel);

/**
 *
 */
saeclib_error_e saeclib_circular_buffer_disposeone(saeclib_circular_buffer_t* buf);

/**
 *
 */
saeclib_error_e saeclib_circular_buffer_disposemany(saeclib_circular_buffer_t* buf,
                                                    uint32_t numel);


typedef struct saeclib_u8_circular_buffer
{
    uint8_t* data;
    int head, tail;
    size_t capacity;
} saeclib_u8_circular_buffer_t;


saeclib_error_e saeclib_u8_circular_buffer_init(saeclib_u8_circular_buffer_t* buf,
                                               void* bufspace,
                                               size_t bufsize);

#define saeclib_u8_circular_buffer_salloc(capacity) \
    ({ \
        saeclib_u8_circular_buffer_t scb; \
        static uint8_t space[(capacity)]; \
        saeclib_u8_circular_buffer_init(&scb, space, (capacity)); \
        scb; \
    })

size_t saeclib_u8_circular_buffer_capacity(const saeclib_u8_circular_buffer_t* buf);
size_t saeclib_u8_circular_buffer_size(const saeclib_u8_circular_buffer_t* buf);
bool saeclib_u8_circular_buffer_empty(const saeclib_u8_circular_buffer_t* buf);

saeclib_error_e saeclib_u8_circular_buffer_pushone(saeclib_u8_circular_buffer_t* buf,
                                                   uint8_t item);
saeclib_error_e saeclib_u8_circular_buffer_pushmany(saeclib_u8_circular_buffer_t* buf,
                                                    const uint8_t* items,
                                                    uint32_t numel);

saeclib_error_e saeclib_u8_circular_buffer_popone(saeclib_u8_circular_buffer_t* buf,
                                                  uint8_t* item);
saeclib_error_e saeclib_u8_circular_buffer_popmany(saeclib_u8_circular_buffer_t* buf,
                                                   uint8_t* items,
                                                   uint32_t numel);

saeclib_error_e saeclib_u8_circular_buffer_peekone(const saeclib_u8_circular_buffer_t* buf,
                                                   uint8_t* item);
saeclib_error_e saeclib_u8_circular_buffer_peekmany(const saeclib_u8_circular_buffer_t* buf,
                                                    uint8_t* item,
                                                    uint32_t numel);

saeclib_error_e saeclib_u8_circular_buffer_disposeone(saeclib_u8_circular_buffer_t* buf);
saeclib_error_e saeclib_u8_circular_buffer_disposemany(saeclib_u8_circular_buffer_t* buf,
                                                       uint32_t numel);


struct saeclib_vp_circular_buffer
{
    void** data;
} saeclib_vp_circular_buffer_t;

#endif
