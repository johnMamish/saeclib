#ifndef _SAECLIB_HASH_H
#define _SAECLIB_HASH_H

#include <stdint.h>
#include <stddef.h>
#include "saeclib_error.h"

/**
 * This hash table uses open addressing to make the statically allocated memory requirement easier
 * to work with.
 *
 * Potentially we could move to a system that has its own memory manager, but that seems like too
 * much work when open addressing works perfectly fine.
 */

typedef struct saeclib_hash_table
{
    // Pointer to the memory region that holds keys and values respectively for this hash table.
    uint8_t* key_data;
    uint8_t* value_data;

    // Array of bools; each entry records whether an entry is nil (0), filled (1), or deleted (2).
    // Note that at the moment, we don't use deleted entries.
    // This could be implemented with a bitpacked array, but I've chosen for the time being to
    // just use bytes.
    uint8_t* bucket_filled;

    // How many elements can be stored in the hash table.
    size_t capacity;

    // How many bytes is each element?
    size_t key_elt_size;
    size_t value_elt_size;

    // Hash function for keys stored within the hash.
    // The hash is passed a pointer to the key.
    unsigned int (*hash_fn)(const void*);

    // Compare function for the keys stored within the hash.
    // It should return 0 if they are equal and non-zero if they are not.
    int (*cmp)(const void*, const void*);
} saeclib_hash_table_t;

/**
 * Initializes an empty saeclib hash table with the given parameters.
 *
 * Because everything in saeclib is statically allocated, array sizes must be calculated manually
 * by the user. See the arg docstrings for more info.
 *
 * @param[in,out] sht       hash table that is to be initialized.
 * @param[in]     keyspace  Pointer to a statically allocated memory region with enough memory for
 *                          holding all keys in the hash. Should be 'capacity * keysize' Bytes.
 * @param[in]     valuespace  Pointer to a statically allocated memory region with enough mem for
 *                            holding all values in the hash. Should be 'capacity * valuesize' Bytes.
 *                            If the hash holds no values, NULL can be passed in.
 * @param[in]     bucketspace Pointer to a statically allocated memory region that contains enough
 *                            memory for recording entry statuses. Should be 'capacity' bytes.
 * @param[in]     keyspace_size   Should pass in sizeof(keyspace). This determines the capacity
 *                                of the hash table.
 * @param[in]     key_size  Size in bytes of each key to be stored in the hash.
 * @param[in]     value_size Size in bytes of each value to be stored in the hash.
 *                           If the has holds only keys and no values, should be passed in.
 * @param[in]     hash_fn   Hash function to use for entries in this hash table.
 * @param[in]     cmp       Compare function for entries in this hash table. See struct decl for
 *                          a more detailed description.
 */
saeclib_error_e saeclib_hash_table_init(saeclib_hash_table_t* sht,
                                        void* keyspace,
                                        void* valuespace,
                                        void* bucketspace,
                                        size_t keyspace_size,
                                        size_t key_size,
                                        size_t value_size,
                                        unsigned int (*hash_fn)(const void*),
                                        int (*cmp)(const void*, const void*));

/**
 * As with all salloc macros in saeclib, use with caution. Containers initialized with a salloc
 * macro must not be passed out of a function that's called more than once.
 *
 * Statically allocates an appropriate amount of memory in an inner scope and then calls
 * saeclib_hash_table_init on it.
 *
 * @param[in]     capacity    How many elements should the has table be able to hold?
 * @param[in]     elt_size    What's the size of each elemtn in bytes?
 * @param[in]     hash_fn     Has function to be applied to each void* element.
 *
 * @return This pacro returns a saeclib_hash_table_t
 *
 * Example usage:
 *     saeclib_hash_table_t sht = saeclib_hash_table_salloc(1024, sizeof(mystruct_t));
 */
#define saeclib_hash_table_salloc(capacity, keysize, valuesize, hash_fn, cmp) \
    ({ \
    saeclib_hash_table_t sht; \
    static uint8_t keyspace[(capacity) * (keysize)] = { 0 };    \
    static uint8_t valuespace[(capacity) * (valuesize)] = { 0 };        \
    static uint8_t status[(capacity)] = { 0 };                          \
    saeclib_hash_table_init(&sht, keyspace, valuespace, status, sizeof(keyspace), \
                            keysize, valuesize, hash_fn, cmp);          \
    sht; \
    })

/**
 * Same as above, except for hash tables with no values, only keys.
 */
#define saeclib_hash_set_salloc(capacity, keysize, hash_fn, cmp) \
      ({ \
      saeclib_hash_table_t sht; \
      static uint8_t keyspace[(capacity) * (keysize)] = { 0 };  \
      static uint8_t status [(capacity)] = { 0 };                       \
      saeclib_hash_table_init(&sht, keyspace, NULL, status, sizeof(keyspace), \
                              keysize, 0, hash_fn, cmp);                \
      sht; \
      })


/**
 * Inserts a new key-value pair into the hash.
 *
 * If an identical key already exists in the
 */
saeclib_error_e saeclib_hash_insert(saeclib_hash_table_t* sht,
                                    const void* key,
                                    const void* value);

/**
 * Finds a key-value pair in the hash and copies the corresponding value into the pointer 'out'.
 *
 * If no assocated key is found, then SAECLIB_ERROR_UNDERFLOW is returned.
 */
saeclib_error_e saeclib_hash_search(saeclib_hash_table_t* sht,
                                    const void* key,
                                    void* out);

/**
 * Same as saeclib_hash_search, but returns a pointer to the value, meaning that it can be modified.
 */
saeclib_error_e saeclib_hash_search_ref(saeclib_hash_table_t* sht,
                                        const void* key,
                                        void** out);

/**
 *
 */
saeclib_error_e saeclib_hash_delete(saeclib_hash_table_t* sht,
                                    const void* key);

////////////////////////////////////////////////////////////////
// courtesy hash and compare functions for u32 and string
unsigned int saeclib_hash_table_u32_hash(const void* a);
int saeclib_hash_table_u32_cmp(const void* a, const void* b);

unsigned int saeclib_hash_table_str_hash(const void* a);
int saeclib_hash_table_str_cmp(const void* a, const void* b);

#endif
