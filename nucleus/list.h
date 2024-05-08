#ifndef NU_LIST_H
#define NU_LIST_H

#include <nucleus/allocator.h>

typedef struct
{
    nu_size_t                 size;
    nu_size_t                 obj_size;
    struct nu__object_header *first;
    struct nu__object_header *head;
    struct nu__object_header *free;
} nu__list_t;

void       nu__list_init(nu__list_t *list, nu_size_t obj_size);
nu_error_t nu__list_append(nu__list_t          *list,
                           nu__allocator_t     *alloc,
                           nu__allocator_flag_t flag,
                           void               **obj);
void       nu__list_remove(nu__list_t *list, void *obj);
void      *nu__list_first(nu__list_t *list);
void      *nu__list_next(nu__list_t *list, void *obj);

#ifdef NU_IMPLEMENTATION

struct nu__object_header
{
    struct nu__object_header *prev; /* only valid in object list */
    struct nu__object_header *next;
};

static void *
nu__object_from_header (struct nu__object_header *header, nu_size_t size)
{
    return (void *)((nu_size_t)header - size);
}

static struct nu__object_header *
nu__header_from_object (void *obj, nu_size_t size)
{
    return (struct nu__object_header *)((nu_size_t)obj + size);
}

void
nu__list_init (nu__list_t *list, nu_size_t obj_size)
{
    list->size     = obj_size + sizeof(struct nu__object_header);
    list->obj_size = obj_size;
    list->first    = NU_NULL;
    list->head     = NU_NULL;
    list->free     = NU_NULL;
}

nu_error_t
nu__list_append (nu__list_t          *list,
                 nu__allocator_t     *alloc,
                 nu__allocator_flag_t flag,
                 void               **obj)
{
    struct nu__object_header *header;
    nu_error_t                error;

    /* create object */
    if (list->free)
    {
        header = list->free;
        /* remove from free list */
        list->free = header->next;
    }
    else
    {
        /* allocate new object */
        void *object;
        error = nu__alloc(alloc, list->size, flag, &object);
        NU_ERROR_CHECK(error, return error);
        header = nu__header_from_object(object, list->obj_size);
    }

    /* add to object list */
    if (list->head)
    {
        list->head->next = header;
        header->prev     = list->head;
    }
    else
    {
        header->prev = NU_NULL;
        list->first  = header;
    }
    header->next = NU_NULL;
    list->head   = header;
    *obj         = nu__object_from_header(header, list->obj_size);
    return NU_ERROR_NONE;
}

void
nu__list_remove (nu__list_t *list, void *obj)
{
    struct nu__object_header *header
        = nu__header_from_object(obj, list->obj_size);

    NU_ASSERT(obj);

    /* remove from object list */
    if (header->prev)
    {
        header->prev->next = header->next;
    }
    if (header->next)
    {
        header->next->prev = header->prev;
    }
    if (list->head == header)
    {
        list->head = header->prev;
    }
    if (list->first == header)
    {
        list->first = header->next;
    }

    /* add to free list */
    header->next = list->free;
    header->prev = NU_NULL;
    list->free   = header;
}

void *
nu__list_first (nu__list_t *list)
{
    if (list->first)
    {
        return nu__object_from_header(list->first, list->obj_size);
    }
    return NU_NULL;
}

void *
nu__list_next (nu__list_t *list, void *obj)
{
    struct nu__object_header *header;

    NU_ASSERT(obj);

    header = nu__header_from_object(obj, list->obj_size);
    if (header->next)
    {
        return nu__object_from_header(header->next, list->obj_size);
    }
    return NU_NULL;
}
#endif

#endif
