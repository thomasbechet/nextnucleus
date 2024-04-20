#ifndef NU_ALLOCATOR_H
#define NU_ALLOCATOR_H

#include <nucleus/types.h>

typedef void *(*nu_allocator_alloc_pfn_t)(nu_size_t size,
                                          nu_size_t align,
                                          void     *userdata);
typedef void (*nu_allocator_free_pfn_t)(void *ptr, void *userdata);

typedef struct
{
    void                    *userdata;
    nu_allocator_alloc_pfn_t alloc;
    nu_allocator_free_pfn_t  free;
} nu__allocator_t;

typedef struct
{
    void                    *userdata;
    nu_allocator_alloc_pfn_t alloc;
    nu_allocator_free_pfn_t  free;
} nu_allocator_info_t;

#endif
