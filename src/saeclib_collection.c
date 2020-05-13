#include "saeclib_collection.h"

#include <string.h>

saeclib_error_e saeclib_collection_init(saeclib_collection_t* collection,
                                        void* bufspace,
                                        size_t bufsize,
                                        size_t eltsize,
                                        saeclib_circular_buffer_t* slots,
                                        uint32_t* bitmap_space)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


size_t saeclib_collection_capacity(const saeclib_collection_t* collection)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


size_t saeclib_collection_size(const saeclib_collection_t* collection)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_add(saeclib_collection_t* collection,
                                       const void* item,
                                       saeclib_collection_iterator_t* it)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_iterator_init(const saeclib_collection_t* collection,
                                                 saeclib_collection_iterator_t* it)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_iterator_next(const saeclib_collection_t* collection,
                                                 saeclib_collection_iterator_t* it)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_iterator_get(const saeclib_collection_t* collection,
                                                const saeclib_collection_iterator_t* it,
                                                void* item)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_iterator_get_volatile(const saeclib_collection_t* collection,
                                                         const saeclib_collection_iterator_t* it,
                                                         void** pitem)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_collection_remove_item(const saeclib_collection_t* collection,
                                               saeclib_collection_iterator_t* it)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}
