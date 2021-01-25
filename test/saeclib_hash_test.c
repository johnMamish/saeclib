#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"

#include "saeclib_hash.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * Test to see if the hash table is being initialized correctly.
 */
void saeclib_hash_table_init_test()
{
#define NUMEL 128

    saeclib_hash_table_t sht;
    static char* keyspace[NUMEL];
    static int valuespace[NUMEL];
    static uint8_t status[NUMEL];

    saeclib_error_e err = saeclib_hash_table_init(&sht, keyspace, valuespace, status,
                                                  sizeof(keyspace), sizeof(char*), sizeof(int),
                                                  saeclib_hash_table_str_hash,
                                                  saeclib_hash_table_str_cmp);

    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_PTR(keyspace, sht.key_data);
    TEST_ASSERT_EQUAL_PTR(valuespace, sht.value_data);
    TEST_ASSERT_EQUAL_PTR(status, sht.bucket_filled);

    TEST_ASSERT_EQUAL_INT(NUMEL, sht.capacity);

#undef NUMEL
}

void saeclib_hash_table_salloc_test()
{
#define NUMEL 128

    saeclib_hash_table_t sht = saeclib_hash_table_salloc(NUMEL, sizeof(char*), sizeof(int),
                                                         saeclib_hash_table_str_hash,
                                                         saeclib_hash_table_str_cmp);

    TEST_ASSERT_EQUAL_INT(NUMEL, sht.capacity);

#undef NUMEL
}

/**
 * Insert a KVP into the hash table and then fetch it.
 */
void saeclib_hash_table_insert_test0()
{
#define NUMEL 128
    saeclib_hash_table_t sht = saeclib_hash_table_salloc(NUMEL, sizeof(char*), sizeof(int),
                                                         saeclib_hash_table_str_hash,
                                                         saeclib_hash_table_str_cmp);

    // Insert
    saeclib_error_e err;
    err = saeclib_hash_insert(&sht, (char*[]){"john"}, (int[]){0});
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);

    // Fetch reference and increment
    int* ref;
    err = saeclib_hash_search_ref(&sht, (char*[]){"john"}, (void**)&ref);
    (*ref)++;
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);

    // Fetch entry
    int a = 0;
    err = saeclib_hash_search(&sht, (char*[]){"john"}, &a);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_INT(1, a);

#undef NUMEL
}

/**
 * Test a failed lookup
 */
void saeclib_hash_table_search_missing_entry0()
{
#define NUMEL 128
    saeclib_hash_table_t sht = saeclib_hash_table_salloc(NUMEL, sizeof(char*), sizeof(int),
                                                         saeclib_hash_table_str_hash,
                                                         saeclib_hash_table_str_cmp);

    // Insert
    saeclib_error_e err;
    err = saeclib_hash_insert(&sht, (char*[]){"john"}, (int[]){0});
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    err = saeclib_hash_insert(&sht, (char*[]){"jill"}, (int[]){0});
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);

    // lookup with failure
    int x = 0x5a5a5a5a;
    err = saeclib_hash_search(&sht, (char*[]){"jack"}, (void*)&x);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
    TEST_ASSERT_EQUAL_INT(0x5a5a5a5a, x);

#undef NUMEL
}


/**
 * force a hash collision
 */




/**
 * Simple fuzz test against a golden example.
 *
 * Randomly adds, removes, or changes increments a value.
 */
void saeclib_hash_table_fuzz_test0()
{
#define NUMEL 1024
    srand(0);

    saeclib_hash_table_t sht = saeclib_hash_table_salloc(NUMEL, sizeof(char*), sizeof(int),
                                                         saeclib_hash_table_u32_hash,
                                                         saeclib_hash_table_u32_cmp);

#define KEY_RANGE 4000
    static int golden_array[KEY_RANGE] = { 0 };
    static int golden_occupancy[KEY_RANGE] = { 0 };
    int numel = 0;

    for (int i = 0; i < 200000; i++) {
        int action = rand() % 100;
        uint32_t key = rand() % KEY_RANGE;
        int value = rand();

        if (action < 10) {
            // action e [0, 10) means we add a kvp to the map.
            if (numel < NUMEL) {
                // make sure we don't overflow it.

                // update our hash
                saeclib_error_e err = saeclib_hash_insert(&sht, &key, &value);

                // update golden example and check ours
                if (golden_occupancy[key]) {
                    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_DUPLICATE_KEY, err);
                } else {
                    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
                    golden_array[key] = value;
                    golden_occupancy[key] = 1;
                    numel++;
                }
            }
        } else if (action < 30) {
            // action e [15, 30) means that we delete a kvp from the map.
            if (numel > 0) {
                // make sure we don't underflow it.

                // update our hash
                saeclib_error_e err = saeclib_hash_delete(&sht, &key);

                // update golden example
                if (golden_occupancy[key]) {
                    numel--;
                    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
                    golden_occupancy[key] = 0;
                } else {
                    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
                }
            }
        } else if (action < 60) {
            // action e [30, 60) means that we try to read a kvp from the map.
            int x;
            saeclib_error_e err = saeclib_hash_search(&sht, &key, &x);

            if (golden_occupancy[key]) {
                TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
                TEST_ASSERT_EQUAL_INT(golden_array[key], x);
            } else {
                TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
            }
        } else {
            // increment the value in the map
            int* px;
            saeclib_error_e err = saeclib_hash_search_ref(&sht, &key, (void**)&px);

            if (golden_occupancy[key]) {
                TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
                TEST_ASSERT_EQUAL_INT(golden_array[key], *px);
                (*px)++;
                golden_array[key]++;
            } else {
                TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
            }
        }
    }

    // do one final test, iterating over every key in the golden array and checking its value
    // against our array.
    for (int i = 0; i < KEY_RANGE; i++) {
        int x;
        saeclib_error_e err = saeclib_hash_search(&sht, &i, &x);

        if (golden_occupancy[i]) {
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
            TEST_ASSERT_EQUAL_INT(golden_array[i], x);
        } else {
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
        }
    }
#undef NUMEL
#undef KEY_RANGE
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(saeclib_hash_table_init_test);
    RUN_TEST(saeclib_hash_table_salloc_test);
    RUN_TEST(saeclib_hash_table_insert_test0);
    RUN_TEST(saeclib_hash_table_search_missing_entry0);
    RUN_TEST(saeclib_hash_table_fuzz_test0);
    return UNITY_END();
}
