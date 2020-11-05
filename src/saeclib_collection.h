#ifndef _SAECLIB_COLLECTION_BUFFER_H
#define _SAECLIB_COLLECTION_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "saeclib_circular_buffer.h"
#include "saeclib_error.h"


/**
 * A saeclib collection is a datatype which holds an unordered list of elements which all have the
 * same size. The elements can be iterated over with an iterator.
 */
typedef struct saeclib_collection
{
    // we use uint8_t as the base type for storing data as it's guaranteed to have sizeof() 1.
    uint8_t* data;

    size_t capacity;
    size_t elt_size;

    // This queue is used for keeping track of open slots. It contains uint32_t that are index
    // numbers
    saeclib_circular_buffer_t* slots;

    // This bitmap is a fast way to determine which slots are occupied when iterating.
    // The LSbit of each word corresponds to lower slot numbers
    uint32_t* occupied_bitmap;
} saeclib_collection_t;


typedef struct saeclib_collection_iterator
{
    int idx;
} saeclib_collection_iterator_t;

/**
 * Because a saeclib collection requires a queue internally, this function is somewhat convoluted
 * to call. The caller is responsible for statically allocating and initializing several structures
 * of specific sizes.
 *
 * @param[in,out] collection  Collection to be initialized with the given statically allocated space
 * @param[in]     bufspace    Pointer to a statically allocated memory region containing enough
 *                            memory to satisfy the collection's items: bufsize * eltsize bytes.
 * @param[in]     bufsize     Size of bufspace in bytes. sizeof(bufspace) should be passed in.
 * @param[in]     eltsize     Size of elements to be stored in the buffer. If you give this
 *                            function a chunk of memory that's not a multiple of elt_size, it
 *                            won't complain, but memory will be wasted.
 * @param[in]     slots       Statically allocated circular buffer to be used by the saeclib
 *                            collection. This buffer should be initialized with element size
 *                            sizeof(uint32_t) and enough space to hold bufsize / eltsize items, its
 *                            capacity should be initialized to the collection's capacity + 1.
 * @param[in]     bitmap_space   Should point to a statically allocated array of uint32 to be used
 *                               by the saeclib collection. It should have one bit for each element
 *                               in the collection (this can be calculated by taking
 *                               (capacity / 32) + 1
 *
 * @return If null pointers are provided, this function returns SAECLIB_ERROR_NULL_POINTER.
 *         If the 'slots' circular buffer isn't initialized correctly, this function returns
 *         SAECLIB_ERROR_BAD_STRUCTURE.
 *         Otherwise, SAECLIB_ERROR_NOERROR is returned.
 */
saeclib_error_e saeclib_collection_init(saeclib_collection_t* collection,
                                        void* bufspace,
                                        size_t bufsize,
                                        size_t eltsize,
                                        saeclib_circular_buffer_t* slots,
                                        uint32_t* bitmap_space);

/**
 * As with all salloc macros in saeclib, use with caution. Containers initialized with a salloc
 * macro must not be passed out of a function that's called more than once.
 *
 * Statically allocates an appropriate amount of memory and initializes structures appropriately in
 * an inner scope and then calls saeclib_collection_init on it.
 *
 * @param[in]     capacity    How many elements should the collection be able to hold?
 * @param[in]     elt_size    What's the size of each element, in bytes?
 *
 * @return This macro passes out a saeclib_collection_t.
 *
 * Example usage:
 *
 *     // create a collection that can hold 16 mystruct_t's.
 *     saeclib_collection_t scl = saeclib_collection_salloc(16, sizeof(mystruct_t));
 */

#define saeclib_collection_salloc(capacity, elt_size) \
    ({ \
    static saeclib_circular_buffer_t scb; \
    scb = saeclib_circular_buffer_salloc((capacity + 1), sizeof(uint32_t)); \
    static uint32_t bitmap_space[(capacity / 32) + 1]; \
    saeclib_collection_t scl; \
    static uint8_t space[(capacity) * (elt_size)]; \
    saeclib_collection_init(&scl, space, (capacity) * (elt_size), (elt_size), &scb, bitmap_space); \
    scl; \
    })


/**
 * Returns the capacity of the collection, in number of elements.
 */
size_t saeclib_collection_capacity(const saeclib_collection_t* collection);

/**
 * Returns the number of elements currently in the collection.
 */
size_t saeclib_collection_size(const saeclib_collection_t* collection);

/**
 * @param[in,out] collection  Collection to which the new item should be added.
 * @param[in]     item        Item to add to the collection
 * @param[out]    it          Optional paramater used to pass out an iterator which can be used to
 *                            access the newly added item. Pass in NULL if the iterator isn't needed
 *
 * @return Returns SAECLIB_ERROR_NOERROR if the item was successfully added. If there is no space
 *         in the collection, returns SAECLIB_ERROR_OVERFLOW.
 */
saeclib_error_e saeclib_collection_add(saeclib_collection_t* collection,
                                       const void* item,
                                       saeclib_collection_iterator_t* it);

/**
 * Initializes a collection iterator to iterate over the items in a collection. If a collection has
 * items added to it after this function is called, it isn't guaranteed that
 *
 * @param[in,out] collection  Collection that the iterator will iterate over.
 * @param[out]    it          Iterator structure to initialize.
 *
 * @return Returns SAECLIB_ERROR_UNDERFLOW if the collection is empty.
 *         Otherwise, returns SAECLIB_ERROR_NOERROR unless a null pointer or malformed structure
 *         is provided.
 */
saeclib_error_e saeclib_collection_iterator_init(const saeclib_collection_t* collection,
                                                 saeclib_collection_iterator_t* it);

/**
 * Increments the iterator by one.
 *
 * @return Returns SAECLIB_ERROR_UNDERFLOW if there are no elements left in the collection.
 *         Returns SAECLIB_ERROR_BAD_STRUCTURE if there's an issue with one of the structs.
 *         Returns SAECLIB_ERROR_NOERROR otherwise.
 */
saeclib_error_e saeclib_collection_iterator_next(const saeclib_collection_t* collection,
                                                 saeclib_collection_iterator_t* it);

/**
 * Copies an item out of the collection into the space pointed by void* item.
 *
 * @param[out]    item        Pointer to space where item will be copied to
 *
 * @return Returns SAECLIB_ERROR_BAD_STRUCTURE if the iterator doesn't point to a valid item in a
 *         collection.
 *         Otherwise, returns SAECLIB_ERROR_NOERROR.
 */
saeclib_error_e saeclib_collection_iterator_get(const saeclib_collection_t* collection,
                                                const saeclib_collection_iterator_t* it,
                                                void* item);

/**
 * Gets a pointer directly to the internal memory structure. If the structure is operated upon
 * after calling this function, the data pointed by the returned pointer may become invalid.
 *
 * @param[out]    pitem       Pointer to a void*; after this function finishes successfully, it will
 *                            point to internal space within the collection where the item
 *                            referenced by the iterator it is stored.
 *
 * @return Returns SAECLIB_ERROR_BAD_STRUCTURE if the iterator doesn't point to a valid item in a
 *         collection.
 *         Otherwise, returns SAECLIB_ERROR_NOERROR.
 */
saeclib_error_e saeclib_collection_iterator_get_volatile(const saeclib_collection_t* collection,
                                                         const saeclib_collection_iterator_t* it,
                                                         void** pitem);

/**
 * Removes an item in the collection referred to by a given iterator.
 */
saeclib_error_e saeclib_collection_remove_item(saeclib_collection_t* collection,
                                               const saeclib_collection_iterator_t* it);

#endif
