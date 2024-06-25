#ifndef NULANG_ALLOCATOR_H
#define NULANG_ALLOCATOR_H

#include <nucleus/vm/types.h>

typedef void *(*nulang_allocator_callback_pfn_t)(nu_size_t size,
                                                 void     *userdata);

typedef struct
{
    nulang_allocator_callback_pfn_t callback;
    void                           *userdata;
} nulang_allocator_info_t;

#ifdef NULANG_IMPL

typedef struct
{
    nulang_allocator_callback_pfn_t callback;
    void                           *userdata;
} nulang__allocator_t;

static void *
nulang__alloc (nulang__allocator_t *alloc, nu_size_t s)
{
    return alloc->callback(s, alloc->userdata);
}

#endif

#endif
