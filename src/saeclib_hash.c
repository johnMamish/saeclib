#include "saeclib_hash.h"
#include <string.h>
#include <assert.h>


static void* get_keyptr_at_idx(saeclib_hash_table_t* sht, int idx)
{
    return sht->key_data + (idx * sht->key_elt_size);
}

static void* get_valptr_at_idx(saeclib_hash_table_t* sht, int idx)
{
    return sht->value_data + (idx * sht->value_elt_size);
}


saeclib_error_e saeclib_hash_table_init(saeclib_hash_table_t* sht,
                                        void* keyspace,
                                        void* valuespace,
                                        void* bucketspace,
                                        size_t keyspace_size,
                                        size_t key_size,
                                        size_t value_size,
                                        unsigned int (*hash_fn)(const void*),
                                        int (*cmp)(const void*, const void*))
{
    sht->key_data = keyspace;

    // If value_size is 0, we point value_data to the same area as key_data; it's never written to,
    // we just need to have a valid pointer in there.
    if (value_size == 0) {
        sht->value_data = keyspace;
    } else {
        sht->value_data = valuespace;
    }

    sht->bucket_filled = bucketspace;

    sht->capacity = keyspace_size / key_size;

    sht->key_elt_size = key_size;
    sht->value_elt_size = value_size;

    sht->hash_fn = hash_fn;
    sht->cmp = cmp;

    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_hash_insert(saeclib_hash_table_t* sht,
                                    const void* key,
                                    const void* value)
{
    // find first empty bucket at our hash location.
    const int start_idx = sht->hash_fn(key) % sht->capacity;
    int idx = start_idx;
    while (1) {
        // if we came across an empty bucket, we can put our data there.
        if (sht->bucket_filled[idx] == 0)
            break;

        // if we came across a bucket that already matches our key, we can't insert.
        if (!sht->cmp(key, get_keyptr_at_idx(sht, idx)))
            return SAECLIB_ERROR_DUPLICATE_KEY;

        // linear probing
        idx = (idx + 1) % sht->capacity;
        if (idx == start_idx)
            return SAECLIB_ERROR_OVERFLOW;
    }

    // copy the key and the data into the bucket
    void* keyptr = get_keyptr_at_idx(sht, idx);
    void* valptr = get_valptr_at_idx(sht, idx);
    memcpy(keyptr, key, sht->key_elt_size);
    memcpy(valptr, value, sht->value_elt_size);

    // mark the bucket as filled.
    sht->bucket_filled[idx] = 1;

    return SAECLIB_ERROR_NOERROR;
}


/**
 * Returns the index of the bucket containing a given key in the hash.
 * If no bucket is found, returns -1.
 */
static int saeclib_hash_search_bucket_idx(saeclib_hash_table_t* sht,
                                          const void* key)
{
    const int start_idx = (int)(sht->hash_fn(key) % sht->capacity);
    int idx = start_idx;
    while (1) {
        // if we came across an empty bucket, we didn't find our key.
        if (sht->bucket_filled[idx] != 1)
            return -1;

        // we know that we aren't looking at an empty bucket, check and see if its a match for our
        // key.
        if (!sht->cmp(key, get_keyptr_at_idx(sht, idx)))
            return idx;

        // linear probing
        idx = (idx + 1) % sht->capacity;

        // if we made it all the way around the array, then we also didn't find our key.
        if (idx == start_idx)
            return -1;
    }

    return idx;
}


saeclib_error_e saeclib_hash_search(saeclib_hash_table_t* sht,
                                    const void* key,
                                    void* out)
{
    void* src;
    saeclib_error_e err = saeclib_hash_search_ref(sht, key, &src);

    if (err != SAECLIB_ERROR_NOERROR)
        return err;

    memcpy(out, src, sht->value_elt_size);
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_hash_search_ref(saeclib_hash_table_t* sht,
                                        const void* key,
                                        void** out)
{
    int bucket_idx = saeclib_hash_search_bucket_idx(sht, key);
    if (bucket_idx == -1)
        return SAECLIB_ERROR_UNDERFLOW;

    *out = get_valptr_at_idx(sht, bucket_idx);
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_hash_delete(saeclib_hash_table_t* sht,
                                    const void* key)
{
    int bucket_idx = saeclib_hash_search_bucket_idx(sht, key);
    if (bucket_idx == -1)
        return SAECLIB_ERROR_UNDERFLOW;

    int del_idx = bucket_idx;
    int rpl_idx = del_idx;

    while (1) {
        // linear probe to find the following index
        rpl_idx = (rpl_idx + 1) % sht->capacity;

        //
        if (sht->bucket_filled[rpl_idx] != 1)
            break;

        // if we loop all the way back around to the start, we can stop
        if (rpl_idx == bucket_idx)
            break;

        // If the 'default' hash position for the replacement candidate falls outside the range
        // between the bucket to be deleted and the bucket from which it will be replaced, a hash
        // chain will broken. The hash chain must be compacted to preserve it.
        //
        // Note that this relies on the mathmatics of linear probing. Modification would be requred
        // for a different probing scheme.
        int replacement_hash = sht->hash_fn(get_keyptr_at_idx(sht, rpl_idx)) % sht->capacity;
        if (rpl_idx < del_idx) {
            // handle wraparound case.
            if ((replacement_hash <= del_idx) && (replacement_hash > rpl_idx)) {
                memcpy(get_keyptr_at_idx(sht, del_idx), get_keyptr_at_idx(sht, rpl_idx),
                       sht->key_elt_size);
                memcpy(get_valptr_at_idx(sht, del_idx), get_valptr_at_idx(sht, rpl_idx),
                       sht->value_elt_size);
                del_idx = rpl_idx;
            }
        } else {
            if ((replacement_hash <= del_idx) || (replacement_hash > rpl_idx)) {
                memcpy(get_keyptr_at_idx(sht, del_idx), get_keyptr_at_idx(sht, rpl_idx),
                       sht->key_elt_size);
                memcpy(get_valptr_at_idx(sht, del_idx), get_valptr_at_idx(sht, rpl_idx),
                       sht->value_elt_size);
                del_idx = rpl_idx;
            }
        }
    }

    // at the end of this shuffle, del_idx points to a cell that can be deleted.
    assert(sht->bucket_filled[del_idx] == 1);
    sht->bucket_filled[del_idx] = 0;

    return SAECLIB_ERROR_NOERROR;
}


unsigned int saeclib_hash_table_u32_hash(const void* a)
{
    return *((uint32_t*)a);
}

int saeclib_hash_table_u32_cmp(const void* a, const void* b)
{
    if (*((uint32_t*)a) > *((uint32_t*)b)) return 1;
    if (*((uint32_t*)a) < *((uint32_t*)b)) return -1;
    return 0;
}

// This is not mine, it is a famous public-domain code snippet from sdbm
unsigned int saeclib_hash_table_str_hash(const void* a)
{
    unsigned int hash = 0;
    int c;
    char* str = *((char**)a);

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

int saeclib_hash_table_str_cmp(const void* a_, const void* b_)
{
    char* a = *((char**)a_);
    char* b = *((char**)b_);

    return strcmp(a, b);
}
