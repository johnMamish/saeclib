#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "saeclib_hash.h"

#define NUMEL 1024
#define KEY_RANGE 2000

void saeclib_hash_table_fuzz_test0()
{
    srand(0);

    saeclib_hash_table_t sht = saeclib_hash_table_salloc(NUMEL, sizeof(char*), sizeof(int),
                                                         saeclib_hash_table_u32_hash,
                                                         saeclib_hash_table_u32_cmp);

    static int golden_array[KEY_RANGE] = { 0 };
    static int golden_occupancy[KEY_RANGE] = { 0 };
    int numel = 0;

    for (int i = 0; i < 1000000; i++) {
        int action = rand() % 100;
        uint32_t key = rand() % KEY_RANGE;
        int value = rand();

        if (action < 10) {
            // action e [0, 15) means we add a kvp to the map.
            if (numel < NUMEL) {
                // make sure we don't overflow it

                //printf("insert key %i\n", key);

                // update our hash
                saeclib_error_e err = saeclib_hash_insert(&sht, &key, &value);

                // update golden example and check ours
                if (golden_occupancy[key]) {
                    assert(SAECLIB_ERROR_DUPLICATE_KEY == err);
                } else {
                    assert(SAECLIB_ERROR_NOERROR == err);
                    golden_array[key] = value;
                    golden_occupancy[key] = 1;
                    numel++;
                }
            }
        } else if (action < 30) {
            // action e [15, 30) means that we delete a kvp from the map.
            if (numel > 0) {
                // make sure we don't underflow it.

                //printf("delete key %i\n", key);

                // update our hash
                saeclib_error_e err = saeclib_hash_delete(&sht, &key);

                // update golden example
                if (golden_occupancy[key]) {
                    numel--;
                    assert(SAECLIB_ERROR_NOERROR == err);
                    golden_occupancy[key] = 0;
                } else {
                    assert(SAECLIB_ERROR_UNDERFLOW == err);
                }
            }
        } else if (action < 60) {
            // action e [30, 60) means that we try to read a kvp from the map.
            int x;
            saeclib_error_e err = saeclib_hash_search(&sht, &key, &x);

            // printf("lookup key %i: ", key);

            if (golden_occupancy[key]) {
                //printf("%5i\n", x);
                assert(SAECLIB_ERROR_NOERROR == err);
                assert(golden_array[key] == x);
            } else {
                //printf("(nil)\n");
                assert(SAECLIB_ERROR_UNDERFLOW == err);
            }
        } else {
            // increment the value in the map
            int* px;
            saeclib_error_e err = saeclib_hash_search_ref(&sht, &key, (void**)&px);

            //printf("incrementing key %i\n", key);

            if (golden_occupancy[key]) {
                assert(SAECLIB_ERROR_NOERROR == err);
                assert(golden_array[key] == *px);
                (*px)++;
                golden_array[key]++;
            } else {
                assert(SAECLIB_ERROR_UNDERFLOW == err);
            }
        }


        // check invariant
        int hash_entry_count = 0;
        for (int i = 0; i < NUMEL; i++) {
            if (sht.bucket_filled[i]) hash_entry_count++;
        }
        printf("hash entry count = %5i, numel = %5i\n", hash_entry_count, numel);
        assert(hash_entry_count == numel);
    }

    // do one final test, iterating over every key in the golden array and checking its value
    // against our array.
    for (int i = 0; i < KEY_RANGE; i++) {
        int x;
        saeclib_error_e err = saeclib_hash_search(&sht, &i, &x);

        if (golden_occupancy[i]) {
            assert(SAECLIB_ERROR_NOERROR == err);
            assert(golden_array[i] == x);
        } else {
            assert(SAECLIB_ERROR_UNDERFLOW == err);
        }
    }
}


int main(int argc, char** argv)
{
    saeclib_hash_table_fuzz_test0();
}
