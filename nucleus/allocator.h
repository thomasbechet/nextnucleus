#ifndef NU_ALLOCATOR_H
#define NU_ALLOCATOR_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/macro.h>
#include <nucleus/memory.h>
#include <nucleus/math.h>

typedef void *(*nu_allocator_alloc_pfn_t)(nu_size_t size,
                                          nu_size_t align,
                                          void     *userdata);
typedef void (*nu_allocator_free_pfn_t)(void *ptr, void *userdata);

typedef enum
{
    NU_ALLOC_FLAG_CORE
} nu__allocator_flag_t;

typedef struct
{
    void *start;
    void *head;
    void *end;
} nu__allocator_t;

nu_error_t nu__allocator_init(void            *vaddr,
                              nu_size_t        capacity,
                              nu__allocator_t *alloc);
void      *nu__alloc(nu__allocator_t     *alloc,
                     nu_size_t            size,
                     nu__allocator_flag_t flag);
void      *nu__aligned_alloc(nu__allocator_t     *alloc,
                             nu_size_t            size,
                             nu_size_t            align,
                             nu__allocator_flag_t flag);

struct
{
    nu_size_t                   free_size;
    struct nu__bucket_header_t *next;
} nu__bucket_header_t;

#ifdef NU_IMPLEMENTATION

nu_error_t
nu__allocator_init (void *vaddr, nu_size_t capacity, nu__allocator_t *alloc)
{
    alloc->start = vaddr;
    alloc->end   = (void *)((nu_size_t)vaddr + (nu_size_t)capacity);
    alloc->head  = vaddr;
    return NU_ERROR_NONE;
}

void *
nu__alloc (nu__allocator_t *alloc, nu_size_t size, nu__allocator_flag_t flag)
{
    void *ptr;
    (void)flag;

    NU_ASSERT(size > 0);

    ptr         = alloc->head;
    alloc->head = (void *)((nu_size_t)ptr + size);

    if (alloc->head > alloc->end)
    {
        return NU_NULL;
    }

    return ptr;
}

void *
nu__aligned_alloc (nu__allocator_t     *alloc,
                   nu_size_t            size,
                   nu_size_t            align,
                   nu__allocator_flag_t flag)
{
    void *ptr;
    (void)flag;

    NU_ASSERT(size > 0);

    ptr         = nu_memalign(alloc->head, align);
    alloc->head = (void *)((nu_size_t)ptr + size);

    if (alloc->head > alloc->end)
    {
        return NU_NULL;
    }

    return ptr;
}

#endif

#endif
