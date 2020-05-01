#include <stdio.h>
#include <stdint.h>

#include "unity.h"

#include "saeclib_circular_buffer.h"

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
 * Test to see if the circular buffer is being initialized correctly.
 */
void saeclib_circular_buffer_init_test()
{
#define NUMEL 10

    saeclib_circular_buffer_t scb;
    static uint8_t bufspace[NUMEL * sizeof(my_struct_t) + 3];

    saeclib_error_e err = saeclib_circular_buffer_init(&scb, bufspace, sizeof(bufspace), sizeof(my_struct_t));

    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_PTR(bufspace, scb.data);
    TEST_ASSERT_EQUAL_INT(scb.head, scb.tail);
    TEST_ASSERT_EQUAL_INT(NUMEL, scb.capacity);
    TEST_ASSERT_EQUAL_INT(sizeof(my_struct_t), scb.elt_size);

    TEST_ASSERT_EQUAL_INT(NUMEL, saeclib_circular_buffer_capacity(&scb));
    TEST_ASSERT_EQUAL_INT(0, saeclib_circular_buffer_size(&scb));
    TEST_ASSERT_TRUE(saeclib_circular_buffer_empty(&scb));

#undef NUMEL
}

/**
 * Check to see if the salloc macro gives us back a correctly initialized buffer.
 */
void saeclib_circular_buffer_salloc_test()
{
#define NUMEL 100

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));
    TEST_ASSERT_NOT_NULL(scb.data);
    TEST_ASSERT_EQUAL_INT(scb.head, scb.tail);
    TEST_ASSERT_EQUAL_INT(NUMEL, scb.capacity);
    TEST_ASSERT_EQUAL_INT(sizeof(my_struct_t), scb.elt_size);

    TEST_ASSERT_EQUAL_INT(NUMEL, saeclib_circular_buffer_capacity(&scb));
    TEST_ASSERT_EQUAL_INT(0, saeclib_circular_buffer_size(&scb));
    TEST_ASSERT_TRUE(saeclib_circular_buffer_empty(&scb));

#undef NUMEL
}

/**
 * Test to see if adding things to the circular buffer increases the size.
 */
void saeclib_circular_buffer_push_count_test()
{
#define NUMEL 10

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));

    const int ELTS_TO_ADD = 6;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_circular_buffer_pushone(&scb, (my_struct_t[]){{ 0 }});
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD, saeclib_circular_buffer_size(&scb));

#undef NUMEL
}

/**
 * Test to see if adding and then removing things from the circular buffer decreases the size.
 */
void saeclib_circular_buffer_push_pop_count_test()
{
#define NUMEL 10

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));

    const int ELTS_TO_ADD = 6;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_circular_buffer_pushone(&scb, (my_struct_t[]){{ 0 }});
    }

    const int ELTS_TO_REMOVE = 4;
    for (int i = 0; i < ELTS_TO_REMOVE; i++) {
        my_struct_t ms;
        saeclib_circular_buffer_popone(&scb, &ms);
    }

    TEST_ASSERT_EQUAL_INT(ELTS_TO_ADD - ELTS_TO_REMOVE, saeclib_circular_buffer_size(&scb));

#undef NUMEL
}

/**
 * Check to see if the empty function correctly determines when the buffer is empty.
 */
void saeclib_circular_buffer_empty_test()
{
#define NUMEL 13

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));

    my_struct_t elements[] = { {0, 10}, {1, 9}, {2, 8}, {3, 7}, {4, 6}, {5, 5}, {6, 4}, {7, 3}, {8, 2}, {9, 1} };

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 5; j++) {
            saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, &elements[j]);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        }

        for (int j = 0; j < 5; j++) {
            my_struct_t temp;
            saeclib_error_e err = saeclib_circular_buffer_popone(&scb, &temp);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
            TEST_ASSERT_EQUAL_MEMORY(&elements[j], &temp, sizeof(my_struct_t));
        }
    }

#undef NUMEL
}

/**
 * Make sure that elements are returned in the right order
 */
void saeclib_circular_buffer_queue_test()
{
#define NUMEL 13

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));

    my_struct_t elements[] = { {0, 10}, {1, 9}, {2, 8}, {3, 7}, {4, 6}, {5, 5}, {6, 4}, {7, 3}, {8, 2}, {9, 1} };

    for (int i = 0; i < NUMEL; i++) {
        for (int j = 0; j < 5; j++) {
            saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, &elements[j]);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        }

        for (int j = 0; j < 5; j++) {
            my_struct_t temp;
            saeclib_error_e err = saeclib_circular_buffer_popone(&scb, &temp);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
            TEST_ASSERT_EQUAL_MEMORY(&elements[j], &temp, sizeof(my_struct_t));
        }
    }

#undef NUMEL
}


/**
 * push and pop a single element many times. Make sure that
 */
void saeclib_circular_buffer_pushone_popone_test()
{
#define NUMEL 13

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));

    my_struct_t elements[] = { {0, 10}, {1, 9}, {2, 8}, {3, 7}, {4, 6}, {5, 5}, {6, 4}, {7, 3}, {8, 2}, {9, 1} };

    my_struct_t temp;
    saeclib_error_e err;
    for (int i = 0; i < (NUMEL * 3); i++) {
        int idx_to_push = i % 10;
        err = saeclib_circular_buffer_pushone(&scb, &elements[idx_to_push]);
        err = saeclib_circular_buffer_popone(&scb, &temp);
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        TEST_ASSERT_EQUAL_MEMORY(&elements[idx_to_push], &temp, sizeof(my_struct_t));
    }

#undef NUMEL
}

void saeclib_circular_buffer_overflow_test()
{
#define NUMEL 13

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(NUMEL, sizeof(my_struct_t));

    const int ELTS_TO_ADD = NUMEL - 1;
    for (int i = 0; i < ELTS_TO_ADD; i++) {
        saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, (my_struct_t[]){{ 0 }});
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    }

    saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, (my_struct_t[]){{ 0 }});
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_OVERFLOW, err);

#undef NUMEL
}

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(saeclib_circular_buffer_init_test);
    RUN_TEST(saeclib_circular_buffer_salloc_test);
    RUN_TEST(saeclib_circular_buffer_push_count_test);
    RUN_TEST(saeclib_circular_buffer_push_pop_count_test);
    RUN_TEST(saeclib_circular_buffer_empty_test);
    RUN_TEST(saeclib_circular_buffer_queue_test);
    RUN_TEST(saeclib_circular_buffer_pushone_popone_test);
    RUN_TEST(saeclib_circular_buffer_overflow_test);

    saeclib_circular_buffer_t scb = saeclib_circular_buffer_salloc(100, sizeof(my_struct_t));

    for (int i = 0; i < 10; i++) {
        my_struct_t ms = {.x = i, .y = 10 - i};
        saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, &ms);

        if (err != SAECLIB_ERROR_NOERROR) {
            printf("unexpected error\n");
        }
    }

    for (int i = 0; i < 10; i++) {
        my_struct_t ms;
        saeclib_error_e err = saeclib_circular_buffer_popone(&scb, &ms);

        if (err != SAECLIB_ERROR_NOERROR) {
            printf("unexpected error\n");
        }
        if ((ms.x != i) || (ms.y != (10 - i))) {
            printf("bad values\n");
        }
    }

    {
        my_struct_t ms;
        saeclib_error_e err = saeclib_circular_buffer_popone(&scb, &ms);

        if (err != SAECLIB_ERROR_UNDERFLOW) {
            printf("underflow not detected\n");
        }
    }

    for (int i = 0; i < 99; i++) {
        my_struct_t ms = {.x = i, .y = 10 - i};
        saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, &ms);

        if (err != SAECLIB_ERROR_NOERROR) {
            printf("unexpected error\n");
        }
    }

    {
        my_struct_t ms = { 0 };
        saeclib_error_e err = saeclib_circular_buffer_pushone(&scb, &ms);

        if (err != SAECLIB_ERROR_OVERFLOW) {
            printf("overflow not detected\n");
        }
    }

    return 0;
}
