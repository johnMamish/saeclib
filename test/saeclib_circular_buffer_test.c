#include <stdio.h>
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

int main(int argc, char** argv)
{
    saeclib_circular_buffer_t scb = saeclib_circular_buffer_t_salloc(100, sizeof(my_struct_t));

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
