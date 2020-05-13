#ifndef _SAECLIB_ERROR_H
#define _SAECLIB_ERROR_H

/**
 * I wasn't sure whether I should have one enum for every different container type or if they could
 * all share an enum. For the sake of ease and readability, for now I am putting all of the
 * possible errors a saeclib call could generate into this enum. Most statically allocated
 * C containers will probably have similar problems anyways.
 */

typedef enum {
    SAECLIB_ERROR_NOERROR = 0,
    SAECLIB_ERROR_UNDERFLOW,
    SAECLIB_ERROR_OVERFLOW,
    SAECLIB_ERROR_BAD_STRUCTURE,
    SAECLIB_ERROR_NULL_POINTER,
    SAECLIB_ERROR_UNKNOWN,
    SAECLIB_ERROR_UNIMPLEMENTED,
} saeclib_error_e;

#endif
