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

#ifdef NU_IMPLEMENTATION

typedef struct
{
    void *start;
    void *head;
    void *end;
} nu__allocator_t;

static nu_error_t
nu__allocator_init (void *vaddr, nu_size_t capacity, nu__allocator_t *alloc)
{
    alloc->start = vaddr;
    alloc->end   = (void *)((nu_size_t)vaddr + (nu_size_t)capacity);
    alloc->head  = vaddr;
    return NU_ERROR_NONE;
}

static nu_error_t
nu__malloc (nu__allocator_t     *alloc,
            nu_size_t            size,
            nu_size_t            align,
            nu__allocator_flag_t flag,
            void               **ptr)
{
    (void)flag;

    NU_ASSERT(size > 0);

    *ptr        = nu_memalign(alloc->head, align);
    alloc->head = (void *)((nu_size_t)*ptr + size);

    if (alloc->head > alloc->end)
    {
        return NU_ERROR_OUT_OF_MEMORY;
    }

    return NU_ERROR_NONE;
}

struct nu__object_footer
{
    struct nu__object_footer *prev;
    struct nu__object_footer *next;
};

static void *
nu__object_from_footer (struct nu__object_footer *footer, nu_size_t size)
{
    return (void *)((nu_size_t)footer - size);
}

typedef struct
{
    nu_size_t                 size;
    struct nu__object_footer *head;
    struct nu__object_footer *tail;
} nu__object_pool_t;

static void nu__object_pool_init(nu__object_pool_t *pool, nu_size_t obj_size)
{
    pool->size = obj_size + sizeof(struct nu__object_footer);
    pool->head = NU_NULL;
    pool->tail = NU_NULL;
}

static void *
nu__object_acquire (nu__object_pool_t *pool, nu__allocator_t *alloc)
{
    if (pool->head)
    {

    }
    return NU_NULL;
}

static void
nu__object_release (nu__object_pool_t *pool, void *obj)
{
}

#endif

#endif
