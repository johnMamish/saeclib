#include <stdio.h>
#include <stdint.h>

#include "unity.h"

#include "saeclib_collection.h"

typedef struct my_struct
{
    int x, y;
} my_struct_t;


void setUp(void)
{
}

void tearDown(void)
{
}


void saeclib_collection_init_test()
{
#define NUMEL 10

    //saeclib_collection_t scoll;
    //static uint8_t a;

    TEST_FAIL_MESSAGE("test unimplemented.");

#undef NUMEL
}


void saeclib_collection_salloc_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add a few elements and see if the count is correct.
 */
void saeclib_collection_add_count_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add, then remove a few elements and see if the count is correct.
 */
void saeclib_collection_add_remove_count_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add then remove an equal number of elements over and over and see if the collection is correctly
 * identified as empty.
 */
void saeclib_collection_empty_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add several elements and make sure that they are all iterated over.
 */
void saeclib_collection_add_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add and then remove several elements. Make sure that the elements which were removed are no
 * longer in the collection.
 */
void saeclib_collection_remove_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add and remove many items from the collection. The test should be structured so that all slots
 * are cycled through.
 */
void saeclib_collection_add_remove_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}

/**
 * Make sure that the collection correctly identifies when it has an overflow condition.
 */
void saeclib_collection_overflow_test()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add items such that the iterator's bitmask is sparse with 1's in both LSB and MSB; make sure that
 * the iterator still gets everything.
 */
void saeclib_collection_iterator_bitpattern_test_1()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


/**
 * Add items so the iterator's bitmask has a few strings of consecutive 1's and 0's
 */
void saeclib_collection_iterator_bitpattern_test_2()
{
    TEST_FAIL_MESSAGE("test unimplemented.");
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(saeclib_collection_init_test);
    RUN_TEST(saeclib_collection_salloc_test);
    RUN_TEST(saeclib_collection_add_count_test);
    RUN_TEST(saeclib_collection_add_remove_count_test);
    RUN_TEST(saeclib_collection_empty_test);
    RUN_TEST(saeclib_collection_add_test);
    RUN_TEST(saeclib_collection_add_remove_test);
    RUN_TEST(saeclib_collection_remove_test);
    RUN_TEST(saeclib_collection_overflow_test);
    RUN_TEST(saeclib_collection_iterator_bitpattern_test_1);
    RUN_TEST(saeclib_collection_iterator_bitpattern_test_2);

    return UNITY_END();
}
