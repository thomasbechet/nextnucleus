#ifndef NU_COMPILER_ALLOCATOR_H
#define NU_COMPILER_ALLOCATOR_H

#include <nucleus/vm/types.h>

typedef void *(*nu_compiler_allocator_callback_pfn_t)(nu_size_t size,
                                                      void     *userdata);

typedef struct
{
    nu_compiler_allocator_callback_pfn_t callback;
    void                                *userdata;
} nu_compiler_allocator_info_t;

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu_compiler_allocator_callback_pfn_t callback;
    void                                *userdata;
} nu__compiler_allocator_t;

static void *
nu__compiler_alloc (nu__compiler_allocator_t *alloc, nu_size_t s)
{
    return alloc->callback(s, alloc->userdata);
}

#endif

#endif
