#include "saeclib_collection.h"

#include <string.h>


/**
 * Maybe this function should return an error in case of out-of-bounds.
 */
static bool get_occupied_bit(const saeclib_collection_t* collection, uint32_t slot)
{
    uint32_t bitmap_idx = slot / 32;
    uint32_t bitmap_bitpos = slot % 32;
    return (collection->occupied_bitmap[bitmap_idx] & (((uint32_t)1) << bitmap_bitpos));
}

/**
 * Maybe this function should return an error in case of out-of-bounds.
 */
static void set_occupied_bit(saeclib_collection_t* collection, uint32_t slot)
{
    uint32_t bitmap_idx = slot / 32;
    uint32_t bitmap_bitpos = slot % 32;
    collection->occupied_bitmap[bitmap_idx] |= (((uint32_t)1) << bitmap_bitpos);
}

/**
 * Maybe this function should return an error in case of out-of-bounds.
 */
static void clear_occupied_bit(saeclib_collection_t* collection, uint32_t slot)
{
    uint32_t bitmap_idx = slot / 32;
    uint32_t bitmap_bitpos = slot % 32;
    collection->occupied_bitmap[bitmap_idx] &= ~(((uint32_t)1) << bitmap_bitpos);
}

saeclib_error_e saeclib_collection_init(saeclib_collection_t* collection,
                                        void* bufspace,
                                        size_t bufsize,
                                        size_t eltsize,
                                        saeclib_circular_buffer_t* slots,
                                        uint32_t* bitmap_space)
{
    collection->data = bufspace;
    collection->capacity = bufsize / eltsize;
    collection->elt_size = eltsize;
    collection->slots = slots;
    collection->occupied_bitmap = bitmap_space;

    // clear bitmap
    memset(collection->occupied_bitmap, 0, ((collection->capacity - 1) / 32) + 1);

    // fill queue
    for (int i = 0; i < collection->capacity; i++) {
        saeclib_circular_buffer_pushone(collection->slots, (uint32_t[]){ i });
    }

    if ((collection->data == NULL) ||
        (collection->slots == NULL) ||
        (collection->occupied_bitmap == NULL)) {
        return SAECLIB_ERROR_NULL_POINTER;
    } else if ((saeclib_circular_buffer_capacity(collection->slots) != (collection->capacity + 1)) ||
               (collection->slots->elt_size != sizeof(uint32_t))) {
        return SAECLIB_ERROR_BAD_STRUCTURE;
    } else {
        return SAECLIB_ERROR_NOERROR;
    }
}


size_t saeclib_collection_capacity(const saeclib_collection_t* collection)
{
    return collection->capacity;
}


size_t saeclib_collection_size(const saeclib_collection_t* collection)
{
    return collection->capacity - saeclib_circular_buffer_size(collection->slots);
}


saeclib_error_e saeclib_collection_add(saeclib_collection_t* scl,
                                       const void* item,
                                       saeclib_collection_iterator_t* it)
{
    // remove a slot from the queue
    uint32_t slot;
    saeclib_error_e queue_err = saeclib_circular_buffer_popone(scl->slots, &slot);

    if (queue_err != SAECLIB_ERROR_NOERROR) {
        if (queue_err == SAECLIB_ERROR_UNDERFLOW) {
            return SAECLIB_ERROR_OVERFLOW;
        } else {
            return SAECLIB_ERROR_UNKNOWN;
        }
    }

    // enforce internal bitmask invariant
    if (get_occupied_bit(scl, slot)) {
        return SAECLIB_ERROR_UNKNOWN;
    }
    set_occupied_bit(scl, slot);

    // copy into array
    void* slotptr = scl->data + (slot * scl->elt_size);
    memcpy(slotptr, item, scl->elt_size);

    return SAECLIB_ERROR_NOERROR;
}


/**
 * returns -1 if 'bits' is all 0's. Otherwise, returns position of first set bit.
 */
static int my_u32_ffs(uint32_t bits)
{
    return __builtin_ffsl(bits) - 1;
}


saeclib_error_e saeclib_collection_iterator_init(const saeclib_collection_t* scl,
                                                 saeclib_collection_iterator_t* it)
{
    // find first element to point iterator at.
    const uint32_t num_bitmask_words = ((scl->capacity - 1) / 32) + 1;

    int bitmask_idx;
    for (bitmask_idx = 0; bitmask_idx < num_bitmask_words; bitmask_idx++) {
        if (scl->occupied_bitmap[bitmask_idx]) {
            break;
        }
    }
    if (bitmask_idx == num_bitmask_words) {
        return SAECLIB_ERROR_UNDERFLOW;
    }

    it->idx = my_u32_ffs(scl->occupied_bitmap[bitmask_idx]) + (bitmask_idx * 32);

    return SAECLIB_ERROR_NOERROR;
}


/**
 * TODO: This function could be made faster by skipping words that are all 0.
 */
saeclib_error_e saeclib_collection_iterator_next(const saeclib_collection_t* scl,
                                                 saeclib_collection_iterator_t* it)
{
    it->idx++;
    while ((it->idx < scl->capacity) && (!get_occupied_bit(scl, it->idx))) {
        it->idx++;
    }

    if (it->idx >= scl->capacity) {
        return SAECLIB_ERROR_UNDERFLOW;
    } else {
        return SAECLIB_ERROR_NOERROR;
    }
}


saeclib_error_e saeclib_collection_iterator_get(const saeclib_collection_t* collection,
                                                const saeclib_collection_iterator_t* it,
                                                void* item)
{
    void* slotptr = collection->data + (it->idx * collection->elt_size);
    memcpy(item, slotptr, collection->elt_size);

    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_collection_iterator_get_volatile(const saeclib_collection_t* collection,
                                                         const saeclib_collection_iterator_t* it,
                                                         void** pitem)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_remove_item(saeclib_collection_t* scl,
                                               const saeclib_collection_iterator_t* it)
{
    // remove a slot from the queue
    uint32_t slot = it->idx;
    saeclib_error_e queue_err = saeclib_circular_buffer_pushone(scl->slots, &slot);

    if (queue_err != SAECLIB_ERROR_NOERROR) {
        return SAECLIB_ERROR_UNKNOWN;
    }

    // enforce internal bitmask invariant
    if (!get_occupied_bit(scl, slot)) {
        return SAECLIB_ERROR_UNKNOWN;
    }
    clear_occupied_bit(scl, slot);

    return SAECLIB_ERROR_NOERROR;
}
