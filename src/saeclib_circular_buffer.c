#include "saeclib_circular_buffer.h"

#include <string.h>


/**
 *
 */
static inline saeclib_error_e try_advance_head(saeclib_circular_buffer_t* buf, size_t steps)
{
    if ((saeclib_circular_buffer_capacity(buf) - saeclib_circular_buffer_size(buf)) <= steps) {
        return SAECLIB_ERROR_OVERFLOW;
    } else {
        buf->head += steps;
        if (buf->head >= saeclib_circular_buffer_capacity(buf)) {
            buf->head -= saeclib_circular_buffer_capacity(buf);
        }
        return SAECLIB_ERROR_NOERROR;
    }
}


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


size_t saeclib_circular_buffer_capacity(const saeclib_circular_buffer_t* buf)
{
    return buf->capacity;
}


size_t saeclib_circular_buffer_size(const saeclib_circular_buffer_t* buf)
{
    if (buf->head >= buf->tail) {
        return (buf->head - buf->tail);
    } else {
        return ((buf->head + buf->capacity) - buf->tail);
    }
}


bool saeclib_circular_buffer_empty(const saeclib_circular_buffer_t* buf)
{
    return (saeclib_circular_buffer_size(buf) == 0);
}


saeclib_error_e saeclib_circular_buffer_pushone(saeclib_circular_buffer_t* buf,
                                                const void* item)
{
    void* oldhead = buf->data + (buf->head * buf->elt_size);
    saeclib_error_e err;

    if ((err = try_advance_head(buf, 1)) != SAECLIB_ERROR_NOERROR) {
        return err;
    } else {
        memcpy(oldhead, item, buf->elt_size);
    }

    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_circular_buffer_pushmany(saeclib_circular_buffer_t* buf,
                                                 const void* items,
                                                 uint32_t numel)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_circular_buffer_popone(saeclib_circular_buffer_t* buf,
                                               void* item)
{
    saeclib_error_e err;

    if ((err = saeclib_circular_buffer_peekone(buf, item)) != SAECLIB_ERROR_NOERROR) {
        return err;
    } else {
        err = saeclib_circular_buffer_disposeone(buf);
    }

    return err;
}


saeclib_error_e saeclib_circular_buffer_popmany(saeclib_circular_buffer_t* buf,
                                                void* items,
                                                uint32_t numel)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_circular_buffer_peekone(const saeclib_circular_buffer_t* buf,
                                                void* item)
{
    void* tailptr = buf->data + (buf->tail * buf->elt_size);
    if (!saeclib_circular_buffer_empty(buf)) {
        memcpy(item, tailptr, buf->elt_size);
        return SAECLIB_ERROR_NOERROR;
    } else {
        return SAECLIB_ERROR_UNDERFLOW;
    }
}


// TODO: remove loop for performance
saeclib_error_e saeclib_circular_buffer_peekmany(const saeclib_circular_buffer_t* buf,
                                                 void* item,
                                                 uint32_t numel)
{
    int idx = buf->tail;
    int cnt = 0;
    while ((idx != buf->head) && (cnt < numel)) {
        void* tailptr = buf->data + (idx * buf->elt_size);
        memcpy((uint8_t*)item + (cnt * buf->elt_size), tailptr, buf->elt_size);
        idx = ((idx + 1) > buf->capacity) ? (0) : (idx + 1);
        cnt++;
    }

    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_circular_buffer_disposeone(saeclib_circular_buffer_t* buf)
{
    return saeclib_circular_buffer_disposemany(buf, 1);
}


saeclib_error_e saeclib_circular_buffer_disposemany(saeclib_circular_buffer_t* buf,
                                                    uint32_t numel)
{
    if (saeclib_circular_buffer_size(buf) < numel) {
        return SAECLIB_ERROR_UNDERFLOW;
    } else {
        buf->tail += numel;
        if (buf->tail >= saeclib_circular_buffer_capacity(buf)) {
            buf->tail -= saeclib_circular_buffer_capacity(buf);
        }
        return SAECLIB_ERROR_NOERROR;
    }
}


// ================================================================

/**
 *
 */
static inline saeclib_error_e try_advance_head_u8(saeclib_u8_circular_buffer_t* buf, size_t steps)
{
    if ((saeclib_u8_circular_buffer_capacity(buf) - saeclib_u8_circular_buffer_size(buf)) <= steps) {
        return SAECLIB_ERROR_OVERFLOW;
    } else {
        buf->head += steps;
        if (buf->head >= saeclib_u8_circular_buffer_capacity(buf)) {
            buf->head -= saeclib_u8_circular_buffer_capacity(buf);
        }
        return SAECLIB_ERROR_NOERROR;
    }
}

saeclib_error_e saeclib_u8_circular_buffer_init(saeclib_u8_circular_buffer_t* buf,
                                             void* bufspace,
                                             size_t bufsize)
{
    buf->data = (uint8_t*)bufspace;
    buf->head = (buf->tail = 0);
    buf->capacity = bufsize;

    return SAECLIB_ERROR_NOERROR;
}


size_t saeclib_u8_circular_buffer_capacity(const saeclib_u8_circular_buffer_t* buf)
{
    return buf->capacity;
}


size_t saeclib_u8_circular_buffer_size(const saeclib_u8_circular_buffer_t* buf)
{
    if (buf->head >= buf->tail) {
        return (buf->head - buf->tail);
    } else {
        return ((buf->head + buf->capacity) - buf->tail);
    }
}


bool saeclib_u8_circular_buffer_empty(const saeclib_u8_circular_buffer_t* buf)
{
    return (saeclib_u8_circular_buffer_size(buf) == 0);
}


saeclib_error_e saeclib_u8_circular_buffer_pushone(saeclib_u8_circular_buffer_t* buf,
                                                   uint8_t item)
{
    uint8_t* oldhead = buf->data + buf->head;
    saeclib_error_e err;

    if ((err = try_advance_head_u8(buf, 1)) != SAECLIB_ERROR_NOERROR) {
        return err;
    } else {
        *oldhead = item;
    }

    return SAECLIB_ERROR_NOERROR;
}


saeclib_error_e saeclib_u8_circular_buffer_pushmany(saeclib_u8_circular_buffer_t* buf,
                                                    const uint8_t* items,
                                                    uint32_t numel)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_u8_circular_buffer_popone(saeclib_u8_circular_buffer_t* buf,
                                                  uint8_t* item)
{
    saeclib_error_e err;

    if ((err = saeclib_u8_circular_buffer_peekone(buf, item)) != SAECLIB_ERROR_NOERROR) {
        return err;
    } else {
        err = saeclib_u8_circular_buffer_disposeone(buf);
    }

    return err;
}


saeclib_error_e saeclib_u8_circular_buffer_popmany(saeclib_u8_circular_buffer_t* buf,
                                                   uint8_t* items,
                                                   uint32_t numel)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_u8_circular_buffer_peekone(const saeclib_u8_circular_buffer_t* buf,
                                                   uint8_t* item)
{
    uint8_t* tailptr = buf->data + buf->tail;
    if (!saeclib_u8_circular_buffer_empty(buf)) {
        *item = *tailptr;
        return SAECLIB_ERROR_NOERROR;
    } else {
        return SAECLIB_ERROR_UNDERFLOW;
    }
}


saeclib_error_e saeclib_u8_circular_buffer_peekmany(const saeclib_u8_circular_buffer_t* buf,
                                                    uint8_t* item,
                                                    uint32_t numel)
{
    return SAECLIB_ERROR_UNIMPLEMENTED;
}


saeclib_error_e saeclib_u8_circular_buffer_disposeone(saeclib_u8_circular_buffer_t* buf)
{
    return saeclib_u8_circular_buffer_disposemany(buf, 1);
}


saeclib_error_e saeclib_u8_circular_buffer_disposemany(saeclib_u8_circular_buffer_t* buf,
                                                       uint32_t numel)
{
    if (saeclib_u8_circular_buffer_size(buf) < numel) {
        return SAECLIB_ERROR_UNDERFLOW;
    } else {
        buf->tail += numel;
        if (buf->tail >= saeclib_u8_circular_buffer_capacity(buf)) {
            buf->tail -= saeclib_u8_circular_buffer_capacity(buf);
        }
        return SAECLIB_ERROR_NOERROR;
    }
}
