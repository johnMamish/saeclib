#include <stdio.h>
#include <stdint.h>

#include "unity.h"

#include "saeclib_circular_buffer.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * Test to see if the circular buffer is being initialized correctly.
 */
void saeclib_u8_circular_buffer_init_test()
{
#define NUMEL 10

    saeclib_u8_circular_buffer_t scb;
    static uint8_t bufspace[NUMEL];

    saeclib_error_e err = saeclib_u8_circular_buffer_init(&scb, bufspace, sizeof(bufspace));

    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_PTR(bufspace, scb.data);
    TEST_ASSERT_EQUAL_INT(scb.head, scb.tail);
    TEST_ASSERT_EQUAL_INT(NUMEL, scb.capacity);

    TEST_ASSERT_EQUAL_INT(NUMEL, saeclib_u8_circular_buffer_capacity(&scb));
    TEST_ASSERT_EQUAL_INT(0, saeclib_u8_circular_buffer_size(&scb));
    TEST_ASSERT_TRUE(saeclib_u8_circular_buffer_empty(&scb));

#undef NUMEL
}

/**
 * Check to see if the salloc macro gives us back a correctly initialized buffer.
 */
void saeclib_u8_circular_buffer_salloc_test()
{
#define NUMEL 100

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);
    TEST_ASSERT_NOT_NULL(scb.data);
    TEST_ASSERT_EQUAL_INT(scb.head, scb.tail);
    TEST_ASSERT_EQUAL_INT(NUMEL, scb.capacity);

    TEST_ASSERT_EQUAL_INT(NUMEL, saeclib_u8_circular_buffer_capacity(&scb));
    TEST_ASSERT_EQUAL_INT(0, saeclib_u8_circular_buffer_size(&scb));
    TEST_ASSERT_TRUE(saeclib_u8_circular_buffer_empty(&scb));

#undef NUMEL
}

/**
 * Test to see if adding things to the circular buffer increases the size.
 */
void saeclib_u8_circular_buffer_push_count_test()
{
#define NUMEL 10

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    const int ELTS_TO_ADD = 6;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_u8_circular_buffer_pushone(&scb, 0);
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD, saeclib_u8_circular_buffer_size(&scb));

#undef NUMEL
}

/**
 * Test to see if adding and then removing things from the circular buffer decreases the size.
 */
void saeclib_u8_circular_buffer_push_pop_count_test()
{
#define NUMEL 10

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    const int ELTS_TO_ADD = 6;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_u8_circular_buffer_pushone(&scb, 0);
    }

    const int ELTS_TO_REMOVE = 4;
    for (int i = 0; i < ELTS_TO_REMOVE; i++) {
        uint8_t u;
        saeclib_u8_circular_buffer_popone(&scb, &u);
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD - ELTS_TO_REMOVE, saeclib_u8_circular_buffer_size(&scb));

#undef NUMEL
}

/**
 * Check to see if the empty function correctly determines when the buffer is empty.
 */
void saeclib_u8_circular_buffer_empty_test()
{
#define NUMEL 13

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint8_t elements[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 5; j++) {
            saeclib_error_e err = saeclib_u8_circular_buffer_pushone(&scb, elements[j]);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        }

        for (int j = 0; j < 5; j++) {
            uint8_t temp;
            saeclib_error_e err = saeclib_u8_circular_buffer_popone(&scb, &temp);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        }
    }

    TEST_ASSERT_TRUE(saeclib_u8_circular_buffer_empty(&scb));

#undef NUMEL
}

/**
 * Make sure that elements are returned in the right order
 */
void saeclib_u8_circular_buffer_queue_test()
{
#define NUMEL 13

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint8_t elements[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    for (int i = 0; i < NUMEL; i++) {
        for (int j = 0; j < 5; j++) {
            saeclib_error_e err = saeclib_u8_circular_buffer_pushone(&scb, elements[j]);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        }

        for (int j = 0; j < 5; j++) {
            uint8_t temp;
            saeclib_error_e err = saeclib_u8_circular_buffer_popone(&scb, &temp);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
            TEST_ASSERT_EQUAL_UINT8(elements[j], temp);
        }
    }

#undef NUMEL
}


/**
 * push and pop a single element many times. Make sure that
 */
void saeclib_u8_circular_buffer_pushone_popone_test()
{
#define NUMEL 13

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint8_t elements[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    uint8_t temp;
    saeclib_error_e err;
    for (int i = 0; i < (NUMEL * 3); i++) {
        int idx_to_push = i % 10;
        err = saeclib_u8_circular_buffer_pushone(&scb, elements[idx_to_push]);
        err = saeclib_u8_circular_buffer_popone(&scb, &temp);
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        TEST_ASSERT_EQUAL_UINT8(elements[idx_to_push], temp);
    }

#undef NUMEL
}

void saeclib_u8_circular_buffer_overflow_test()
{
#define NUMEL 13

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    const int ELTS_TO_ADD = NUMEL - 1;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_error_e err = saeclib_u8_circular_buffer_pushone(&scb, 0);
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    }

    saeclib_error_e err = saeclib_u8_circular_buffer_pushone(&scb, 0);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_OVERFLOW, err);

#undef NUMEL
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(saeclib_u8_circular_buffer_init_test);
    RUN_TEST(saeclib_u8_circular_buffer_salloc_test);
    RUN_TEST(saeclib_u8_circular_buffer_push_count_test);
    RUN_TEST(saeclib_u8_circular_buffer_push_pop_count_test);
    RUN_TEST(saeclib_u8_circular_buffer_empty_test);
    RUN_TEST(saeclib_u8_circular_buffer_queue_test);
    RUN_TEST(saeclib_u8_circular_buffer_pushone_popone_test);
    RUN_TEST(saeclib_u8_circular_buffer_overflow_test);
    return UNITY_END();
}
