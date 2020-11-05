#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "saeclib_circular_buffer.h"
#include "saeclib_collection.h"
#include "saeclib_error.h"


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


/**
 * TODO: write functions to remove elements in an integer list
 */
//static void remove_items_with_value(saeclib_collection_t* scl, uint32_t* blacklist, int numel);

static bool list_contains_u32(uint32_t n, uint32_t* list, int numel)
{
    bool ret = false;
    for (int i = 0; i < numel; i++) {
        if (list[i] == n) {
            ret = true;
        }
    }

    return ret;
}


/**
 * Checks the consistency of the bitmap to make sure that every zero bit in the bitmap is contained
 * in the "open slots" queue and that every one is the bitmap is not in the open slots queue.
 *
 * returns true on success and false on failure.
 */
static bool saeclib_collection_bitmap_invariant_check(const saeclib_collection_t* scl)
{
    uint32_t* open_slots = calloc(scl->capacity, sizeof(uint32_t));
    saeclib_circular_buffer_peekmany(scl->slots, open_slots, saeclib_circular_buffer_size(scl->slots));

    // O(n^2)
    bool queue_and_bitmap_consistent = true;
    for (int slot = 0; slot < scl->capacity; slot++) {
        bool slot_occupied_bit = (scl->occupied_bitmap[slot / 32]) & ((uint32_t)1 << (slot % 32));

        bool slot_in_queue = false;
        for (int i = 0; i < scl->capacity; i++) {
            if (open_slots[i] == slot) {
                slot_in_queue = true;
            }
        }

        if (slot_in_queue && slot_occupied_bit) {
            queue_and_bitmap_consistent = false;
        }
    }

    return queue_and_bitmap_consistent;
}


void saeclib_collection_init_test()
{
#define NUMEL 95

    saeclib_collection_t scl;
    static uint8_t bufspace[sizeof(my_struct_t) * NUMEL];
    static uint32_t bitmap[NUMEL / 32 + 1];
    saeclib_circular_buffer_t slots = saeclib_circular_buffer_salloc(NUMEL + 1, sizeof(uint32_t));

    saeclib_error_e err = saeclib_collection_init(&scl,
                                                  bufspace,
                                                  sizeof(bufspace),
                                                  sizeof(my_struct_t),
                                                  &slots,
                                                  bitmap);

    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_PTR(bufspace, scl.data);
    TEST_ASSERT_EQUAL_INT(NUMEL, scl.capacity);
    TEST_ASSERT_EQUAL_INT(sizeof(my_struct_t), scl.elt_size);
    for (int i = 0; i <= NUMEL / 32; i++) {
        TEST_ASSERT_EQUAL_HEX(0, scl.occupied_bitmap[i]);
    }
    TEST_ASSERT_EQUAL_INT(NUMEL, saeclib_circular_buffer_size(scl.slots));

#undef NUMEL
}


void saeclib_collection_salloc_test()
{
#define NUMEL 95

    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(my_struct_t));

    TEST_ASSERT_EQUAL_INT(NUMEL, scl.capacity);
    TEST_ASSERT_EQUAL_INT(sizeof(my_struct_t), scl.elt_size);
    for (int i = 0; i <= NUMEL / 32; i++) {
        TEST_ASSERT_EQUAL_HEX(0, scl.occupied_bitmap[i]);
    }
    TEST_ASSERT_EQUAL_INT(NUMEL, saeclib_circular_buffer_size(scl.slots));

#undef NUMEL
}


/**
 * Add a few elements and see if the count is correct.
 */
void saeclib_collection_add_count_test()
{
#define NUMEL 95
    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(my_struct_t));

    const int ELTS_TO_ADD = 65;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_collection_add(&scl, (my_struct_t[]){{ 0 }}, NULL);
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD, saeclib_collection_size(&scl));
#undef NUMEL
}


/**
 * Add, then remove a few elements and see if the count is correct.
 */
void saeclib_collection_add_remove_count_test()
{
#define NUMEL 95
    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(my_struct_t));

    const int ELTS_TO_ADD = 65;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_collection_add(&scl, (my_struct_t[]){{ 0 }}, NULL);
    }

    const int ELTS_TO_REMOVE = 32;
    saeclib_collection_iterator_t scit;
    saeclib_collection_iterator_init(&scl, &scit);
    for (int i = 0; i < ELTS_TO_REMOVE; i++) {
        saeclib_collection_remove_item(&scl, &scit);
        saeclib_collection_iterator_next(&scl, &scit);
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD - ELTS_TO_REMOVE, saeclib_collection_size(&scl));
    TEST_ASSERT_TRUE_MESSAGE(saeclib_collection_bitmap_invariant_check(&scl), "Bitmap invariant check failed");
#undef NUMEL
}


/**
 * Add, then remove a few elements and see if the count is correct. This time, skip elements as they
 * are removed.
 */
void saeclib_collection_add_remove_skip_count_test()
{
#define NUMEL 95
    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(my_struct_t));

    const int ELTS_TO_ADD = 65;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_collection_add(&scl, (my_struct_t[]){{ 0 }}, NULL);
    }

    const int ELTS_TO_REMOVE = 30;
    saeclib_collection_iterator_t scit;
    saeclib_collection_iterator_init(&scl, &scit);
    for (int elts_removed = 0, i = 0; elts_removed < ELTS_TO_REMOVE; i++) {
        if (i % 2) {
            saeclib_collection_remove_item(&scl, &scit);
            elts_removed++;
        }
        saeclib_collection_iterator_next(&scl, &scit);
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD - ELTS_TO_REMOVE, saeclib_collection_size(&scl));
    TEST_ASSERT_TRUE_MESSAGE(saeclib_collection_bitmap_invariant_check(&scl), "Bitmap invariant check failed");
#undef NUMEL
}


/**
 * Add then remove an equal number of elements over and over and see if the collection is correctly
 * identified as empty.
 */
void saeclib_collection_empty_test()
{
#define NUMEL 95
    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(my_struct_t));

    const int NUM_ITERATIONS = 6;
    const int ELTS_TO_ADD[] = {65, 55, 45, 35, 15, 25};
    const int ELTS_TO_REMOVE[] = {45, 65, 15, 35, 25, 55};

    // sanity check the number of elements we are asking to add and remove.
    {
        int size = 0;
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            size += ELTS_TO_ADD[i];
            TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(NUMEL, size, "Internal test error");
            size -= ELTS_TO_REMOVE[i];
            TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(0, size, "Internal test error");
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, size, "Internal test error");
    }

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        for (int j = 0; j < ELTS_TO_ADD[i]; j++) {
            saeclib_collection_add(&scl, (my_struct_t[]){{ 0 }}, NULL);
        }

        saeclib_collection_iterator_t scit;
        saeclib_collection_iterator_init(&scl, &scit);
        for (int j = 0; j < ELTS_TO_REMOVE[i]; j++) {
            saeclib_collection_remove_item(&scl, &scit);
            saeclib_collection_iterator_next(&scl, &scit);
        }
    }

    TEST_ASSERT_EQUAL_INT(0, saeclib_collection_size(&scl));
    TEST_ASSERT_TRUE_MESSAGE(saeclib_collection_bitmap_invariant_check(&scl), "Bitmap invariant check failed");
#undef NUMEL
}


/**
 * Add several elements and make sure that they are all iterated over.
 */
void saeclib_collection_add_test()
{
#define NUMEL 95
    const int ELTS_TO_ADD = 65;
    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(int));

    int checklist[ELTS_TO_ADD];
    memset(checklist, 0, sizeof(checklist));

    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_collection_add(&scl, (int[]){i}, NULL);
    }

    saeclib_collection_iterator_t scit;
    saeclib_collection_iterator_init(&scl, &scit);
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        int item = -1;
        saeclib_collection_iterator_get(&scl, &scit, &item);
        saeclib_collection_iterator_next(&scl, &scit);

        TEST_ASSERT_LESS_THAN_INT(ELTS_TO_ADD, item);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, item);

        TEST_ASSERT_EQUAL_INT(0, checklist[item]);
        checklist[item] = 1;
    }

    for (int i = 0; i < ELTS_TO_ADD; i++) {
        TEST_ASSERT_EQUAL_INT(1, checklist[i]);
    }
#undef NUMEL
}


/**
 * Add and then remove several elements. Make sure that the elements which were removed are no
 * longer in the collection.
 */
void saeclib_collection_remove_test()
{
#define NUMEL 95
    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(int));

    const int ELTS_TO_ADD = 65;
    int checklist[ELTS_TO_ADD];
    memset(checklist, 0, sizeof(checklist));

    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_collection_add(&scl, (int[]){i}, NULL);
        checklist[i] = 1;
    }

    uint32_t remove_list[] = { 00,  1,  2,  3,  4,  5,  6,  7,
                               10, 12, 14, 16, 18, 20, 22, 24,
                               32, 33, 34, 35 };
    int elements_to_remove = (sizeof(remove_list) / (sizeof(remove_list[0])));

    saeclib_collection_iterator_t scit;
    saeclib_collection_iterator_init(&scl, &scit);
    do {
        int item;
        saeclib_collection_iterator_get(&scl, &scit, &item);
        if (list_contains_u32(item, remove_list, elements_to_remove)) {
            saeclib_collection_remove_item(&scl, &scit);
            checklist[item] = 0;
        }
    } while (saeclib_collection_iterator_next(&scl, &scit) == SAECLIB_ERROR_NOERROR);

    saeclib_collection_iterator_init(&scl, &scit);
    do {
        int item;
        saeclib_collection_iterator_get(&scl, &scit, &item);

        TEST_ASSERT_EQUAL_INT(1, checklist[item]);
        checklist[item] = 0;
    } while (saeclib_collection_iterator_next(&scl, &scit) == SAECLIB_ERROR_NOERROR);

    for (int i = 0; i < ELTS_TO_ADD; i++) {
        TEST_ASSERT_EQUAL_INT(0, checklist[i]);
    }
#undef NUMEL
}


/**
 * Add and remove many items from the collection. The test should be structured so that all slots
 * are cycled through.
 */
void saeclib_collection_add_remove_test()
{
#define NUMEL 95

    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(int));

    const int NUM_ITERATIONS = 3;
    const int ELTS_TO_ADD[] = {65, 5, 85};
    const int ELTS_TO_REMOVE[] = {35, 25, 95};

    // sanity check the number of elements we are asking to add and remove.
    {
        int size = 0;
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            size += ELTS_TO_ADD[i];
            TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(NUMEL, size, "Internal test error");
            size -= ELTS_TO_REMOVE[i];
            TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(0, size, "Internal test error");
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, size, "Internal test error");
    }

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        for (int j = 0; j < ELTS_TO_ADD[i]; j++) {
            saeclib_collection_add(&scl, (uint32_t[]){ i }, NULL);
        }

        saeclib_collection_iterator_t scit;
        saeclib_collection_iterator_init(&scl, &scit);
        for (int j = 0; j < ELTS_TO_REMOVE[i]; j++) {
            saeclib_collection_remove_item(&scl, &scit);
            saeclib_collection_iterator_next(&scl, &scit);
        }
    }

    for (int i = 0; i <= NUMEL / 32; i++) {
        TEST_ASSERT_EQUAL_INT(0, scl.occupied_bitmap[i]);
    }
    TEST_ASSERT_EQUAL_INT(0, saeclib_collection_size(&scl));

#undef NUMEL
}

/**
 * Make sure that the collection correctly identifies when it has an overflow condition.
 */
void saeclib_collection_overflow_test()
{
#define NUMEL 95

    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(int));

    saeclib_error_e err;
    for (int i = 0; i < NUMEL; i++) {
        err = saeclib_collection_add(&scl, (uint32_t[]){ i }, NULL);
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    }
    err = saeclib_collection_add(&scl, (uint32_t[]){ 0 }, NULL);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_OVERFLOW, err);

#undef NUMEL
}


/**
 * Add items such that the iterator's bitmask is sparse with 1's in both LSB and MSB; make sure that
 * the iterator still gets everything.
 */
void saeclib_collection_iterator_bitpattern_test_1()
{
#define NUMEL 95

    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(int));

    for (int i = 0; i < NUMEL; i++) {
        saeclib_collection_add(&scl, (uint32_t[]){ i }, NULL);
    }

    uint32_t things_to_not_remove[] = {  5, 15, 24, 25,
                                         64,65, 66, 68, 69,
                                         72,73, 75 };
    const int NUMEL_TO_NOT_REMOVE = sizeof(things_to_not_remove) / sizeof(things_to_not_remove[0]);
    saeclib_collection_iterator_t scit;
    saeclib_collection_iterator_init(&scl, &scit);
    do {
        int item;
        saeclib_collection_iterator_get(&scl, &scit, &item);
        if (!list_contains_u32(item, things_to_not_remove, NUMEL_TO_NOT_REMOVE)) {
            saeclib_collection_remove_item(&scl, &scit);
        }
    } while (saeclib_collection_iterator_next(&scl, &scit) == SAECLIB_ERROR_NOERROR);

    uint32_t checklist[NUMEL] = { 0 };
    saeclib_collection_iterator_init(&scl, &scit);
    do {
        int item;
        saeclib_collection_iterator_get(&scl, &scit, &item);
        if (!list_contains_u32(item, things_to_not_remove, NUMEL_TO_NOT_REMOVE)) {
            TEST_FAIL_MESSAGE("collection contains item that should have been removed");
        }
        checklist[item] = 1;
    } while (saeclib_collection_iterator_next(&scl, &scit) == SAECLIB_ERROR_NOERROR);

    for (int i = 0; i < NUMEL_TO_NOT_REMOVE; i++) {
        if (checklist[things_to_not_remove[i]] == 0) {
            TEST_FAIL_MESSAGE("collection did not contain item that still should have been there");
        }
    }

#undef NUMEL
}


/**
 * Add items so the iterator's bitmask has a few strings of consecutive 1's and 0's
 */
void saeclib_collection_iterator_bitpattern_test_2()
{
#define NUMEL 95

    saeclib_collection_t scl = saeclib_collection_salloc(NUMEL, sizeof(int));

    for (int i = 0; i < NUMEL; i++) {
        saeclib_collection_add(&scl, (uint32_t[]){ i }, NULL);
    }

    uint32_t things_to_not_remove[] = {  00,  1,  2,  3,  4,  5,
                                                         12, 13, 14, 15,

                                                                 30, 31,
                                         32, 33, 34, 35, 38, 39,
                                         40, 41, 42, 43, 44, 45,


                                         64, 65,     67, 68, 69,
                                         72, 73, 75, };

    const int NUMEL_TO_NOT_REMOVE = sizeof(things_to_not_remove) / sizeof(things_to_not_remove[0]);
    saeclib_collection_iterator_t scit;
    saeclib_collection_iterator_init(&scl, &scit);
    do {
        int item;
        saeclib_collection_iterator_get(&scl, &scit, &item);
        if (!list_contains_u32(item, things_to_not_remove, NUMEL_TO_NOT_REMOVE)) {
            saeclib_collection_remove_item(&scl, &scit);
        }
    } while (saeclib_collection_iterator_next(&scl, &scit) == SAECLIB_ERROR_NOERROR);

    uint32_t checklist[NUMEL] = { 0 };
    saeclib_collection_iterator_init(&scl, &scit);
    do {
        int item;
        saeclib_collection_iterator_get(&scl, &scit, &item);
        if (!list_contains_u32(item, things_to_not_remove, NUMEL_TO_NOT_REMOVE)) {
            TEST_FAIL_MESSAGE("collection contains item that should have been removed");
        }
        checklist[item] = 1;
    } while (saeclib_collection_iterator_next(&scl, &scit) == SAECLIB_ERROR_NOERROR);

    for (int i = 0; i < NUMEL_TO_NOT_REMOVE; i++) {
        if (checklist[things_to_not_remove[i]] == 0) {
            TEST_FAIL_MESSAGE("collection did not contain item that still should have been there");
        }
    }
#undef NUMEL
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
