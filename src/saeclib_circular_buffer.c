#include "saeclib_circular_buffer.h"

saeclib_error_e saeclib_circular_buffer_init(saeclib_circular_buffer_t* buf,
                                             void* bufspace,
                                             size_t bufsize,
                                             size_t eltsize)
{
    buf->data = (uint8_t*)bufspace;
    buf->head = (buf->tail = 0);
    buf->capacity = bufsize / eltsize;
    buf->elt_size = eltsize;

    return SAECLIB_ERROR_NOERROR;
}


size_t saeclib_circular_buffer_capacity(saeclib_circular_buffer_t* buf)
{
    return buf->capacity;
}


size_t saeclib_circular_buffer_size(saeclib_circular_buffer_t* buf)
{
    if (buf->head >= buf->tail) {
        return (buf->head - buf->tail);
    } else {
        return ((buf->head + buf->capacity) - buf->tail);
    }
}


bool saeclib_circular_buffer_empty(saeclib_circular_buffer_t* buf)
{
    return (saeclib_circular_buffer_size(buf) == 0);
}


saeclib_error_e saeclib_circular_buffer_pushone(saeclib_circular_buffer_t* buf,
                                                void* item)
{
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_pushmany(saeclib_circular_buffer_t* buf,
                                                 void* items,
                                                 uint32_t numel)
{
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_popone(saeclib_circular_buffer_t* buf,
                                               void* item)
{
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_popmany(saeclib_circular_buffer_t* buf,
                                                void* items,
                                                uint32_t numel)
{
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_peekone(const saeclib_circular_buffer_t* buf,
                                                void* item)
{
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_peekmany(const saeclib_circular_buffer_t* buf,
                                                 void* item,
                                                 uint32_t numel)
{
    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_disposeone(saeclib_circular_buffer_t* buf)
{
    return SAECLIB_ERROR_NOERROR;
}

saeclib_error_e saeclib_circular_buffer_disposemany(saeclib_circular_buffer_t* buf,
                                                    uint32_t numel)
{
    return SAECLIB_ERROR_NOERROR;
}
