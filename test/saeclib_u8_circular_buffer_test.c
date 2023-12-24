#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
 * Test to see if bulk adding things to the circular buffer increases the size.
 */
void saeclib_u8_circular_buffer_pushpopmany_count_test()
{
#define NUMEL 10

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);
    uint8_t ones[5] = { 1, 1, 1, 1, 1 };

    saeclib_error_e err = saeclib_u8_circular_buffer_pushmany(&scb, ones, sizeof(ones));
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_INT(5, saeclib_u8_circular_buffer_size(&scb));

    // should copy data rather than pointing to the original
    for (size_t i = 0; i < 5; i++) {
        ones[i] = 2;
    }

    uint8_t out[NUMEL] = { 0 };
    err = saeclib_u8_circular_buffer_popmany(&scb, out, 5);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_INT(0, saeclib_u8_circular_buffer_size(&scb));
    for (size_t j = 0; j < 5; j++) {
        TEST_ASSERT_EQUAL_INT(1, out[j]); // not 2
    }

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

void saeclib_u8_circular_buffer_pushpopmany_wrap_test()
{
#define NUMEL 13

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint8_t in[7] = { 0, 1, 2, 3, 4, 5, 6 };
    uint8_t out[7];

    saeclib_error_e err;
    // push and then pop 7 elements from the 13-capacity buffer 5 times. This should cause several
    // wrap arounds
    for (size_t i = 0; i < 5; i++) {
        err = saeclib_u8_circular_buffer_pushmany(&scb, in, 7);
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        TEST_ASSERT_EQUAL_INT(7, saeclib_u8_circular_buffer_size(&scb));

        err = saeclib_u8_circular_buffer_popmany(&scb, out, 7);
        TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        TEST_ASSERT_EQUAL_INT(0, saeclib_u8_circular_buffer_size(&scb));
    }

    for (size_t j = 0; j < 7; j++) {
        TEST_ASSERT_EQUAL_INT(j, out[j]);
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

void saeclib_u8_circular_buffer_pushmany_overflow_test()
{
#define NUMEL 13

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint8_t data[7] = { 0 };
    saeclib_error_e err = saeclib_u8_circular_buffer_pushmany(&scb, data, 7);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_INT(7, saeclib_u8_circular_buffer_size(&scb));

    err = saeclib_u8_circular_buffer_pushmany(&scb, data, 7);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_OVERFLOW, err);
    TEST_ASSERT_EQUAL_INT(7, saeclib_u8_circular_buffer_size(&scb)); // data should not have changed

#undef NUMEL
}

void saeclib_u8_circular_buffer_popmany_underflow_test()
{
#define NUMEL 10

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint8_t data[5] = { 1 };
    saeclib_error_e err = saeclib_u8_circular_buffer_pushmany(&scb, data, 5);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_INT(5, saeclib_u8_circular_buffer_size(&scb));

    uint8_t out[5] = { 0 };

    err = saeclib_u8_circular_buffer_popmany(&scb, out, 5);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
    TEST_ASSERT_EQUAL_INT(0, saeclib_u8_circular_buffer_size(&scb));

    err = saeclib_u8_circular_buffer_popmany(&scb, out, 5);
    TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
    TEST_ASSERT_EQUAL_INT(0, saeclib_u8_circular_buffer_size(&scb));

#undef NUMEL
}

void saeclib_u8_circular_buffer_pushmany_popmany_fuzz_test0()
{
#define NUMEL 129
    srand(0);

    saeclib_u8_circular_buffer_t scb = saeclib_u8_circular_buffer_salloc(NUMEL);

    uint32_t push_base = 0;
    uint32_t incr_count = 0;
    uint32_t golden_total = 0;
    uint32_t running_total = 0;

    for (int i = 0; i < 100000; i++) {
        int action = rand() % 100;

        if (action < 45) {
            // action e [0, 45) means that we push a random number of elements that won't cause an
            // overflow.
            const int remaining_space =
                (saeclib_u8_circular_buffer_capacity(&scb) - saeclib_u8_circular_buffer_size(&scb) - 1);
            if (remaining_space == 0)
                continue;
            int push_num = rand() % remaining_space;

            // we want to usually use small numbers; work in short chunks most of the time
            if ((rand() % 100) < 90) {
                push_num >>= 1;
                if (push_num == 0) push_num = 1;
            }

            // construct the buffer from which to push
            uint8_t pushbuf[NUMEL] = { 0 };
            for (int j = 0; j < push_num; j++) pushbuf[j] = (push_base + j);
            push_base += push_num;

            // push
            saeclib_error_e err = saeclib_u8_circular_buffer_pushmany(&scb, pushbuf, push_num);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);
        } else if (action < 90) {
            // action e [45, 90), we pop a random number of elements that won't cause an underflow.
            // we also validate that we popped the correct data.
            const int used_space = saeclib_u8_circular_buffer_size(&scb);
            if (used_space == 0)
                continue;
            int pop_num = rand() % used_space;

            // we want to usually use small numbers; work in short chunks most of the time
            if ((rand() % 100) < 90) {
                pop_num >>= 1;
                if (pop_num == 0) pop_num = 1;
            }

            // pop the data
            uint8_t popbuf[NUMEL] = { 0 };
            saeclib_error_e err = saeclib_u8_circular_buffer_popmany(&scb, popbuf, pop_num);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_NOERROR, err);

            // update golden example
            for (int j = 0; j < pop_num; j++) golden_total += (uint8_t)(incr_count + j);
            incr_count += pop_num;

            // update our running total and compare with golden example.
            for (int j = 0; j < pop_num; j++) running_total += popbuf[j];
            TEST_ASSERT_EQUAL_INT(golden_total, running_total);
        } else if (action < 95) {
            // action e [90, 95) force a buffer overflow
            const int remaining_space =
                (saeclib_u8_circular_buffer_capacity(&scb) - saeclib_u8_circular_buffer_size(&scb) - 1);
            uint8_t push_buf[NUMEL] = { 0 };
            saeclib_error_e err = saeclib_u8_circular_buffer_pushmany(&scb, push_buf,
                                                                      remaining_space + 1);
            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_OVERFLOW, err);
        } else if (action < 100) {
            // action e [95, 100) force a buffer underflow
            const int used_space = saeclib_u8_circular_buffer_size(&scb);
            uint8_t pop_buf[NUMEL] = { 0 };
            saeclib_error_e err = saeclib_u8_circular_buffer_popmany(&scb, pop_buf,
                                                                     used_space + 1);

            TEST_ASSERT_EQUAL_INT(SAECLIB_ERROR_UNDERFLOW, err);
        }
    }
#undef NUMEL
}

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(saeclib_u8_circular_buffer_init_test);
    RUN_TEST(saeclib_u8_circular_buffer_salloc_test);
    RUN_TEST(saeclib_u8_circular_buffer_push_count_test);
    RUN_TEST(saeclib_u8_circular_buffer_push_pop_count_test);
    RUN_TEST(saeclib_u8_circular_buffer_pushpopmany_count_test);
    RUN_TEST(saeclib_u8_circular_buffer_empty_test);
    RUN_TEST(saeclib_u8_circular_buffer_queue_test);
    RUN_TEST(saeclib_u8_circular_buffer_pushone_popone_test);
    RUN_TEST(saeclib_u8_circular_buffer_pushpopmany_wrap_test);
    RUN_TEST(saeclib_u8_circular_buffer_overflow_test);
    RUN_TEST(saeclib_u8_circular_buffer_pushmany_overflow_test);
    RUN_TEST(saeclib_u8_circular_buffer_popmany_underflow_test);
    RUN_TEST(saeclib_u8_circular_buffer_pushmany_popmany_fuzz_test0);
    return UNITY_END();
}
