#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/ecs/api.h>
#include <nucleus/ecs/system.h>
#include <nucleus/ecs/component.h>
#include <nucleus/list.h>

typedef struct
{
    /* nu__component_id_t id; */
    void *data;
} nu__archetype_entry_t;

typedef struct
{
    nu_size_t             entry_count;
    nu__archetype_entry_t entries[1];
} nu__archetype_t;

struct nu__query
{
    nu__archetype_t *archetype;
};

/* typedef struct
{

} nu_query_iter_t; */

typedef struct
{
    nu__list_t archetypes;
    nu__list_t components;
    nu__list_t properties;
} nu__ecs_t;

nu_error_t nu__ecs_init(nu__allocator_t *alloc, nu__ecs_t *ecs);
nu_error_t nu__ecs_free(nu__ecs_t *ecs);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu__ecs_init (nu__allocator_t *alloc, nu__ecs_t *ecs)
{
    (void)alloc;
    (void)ecs;

    nu__list_init(&ecs->properties, sizeof(struct nu__property));

    return NU_ERROR_NONE;
}

nu_error_t
nu__ecs_free (nu__ecs_t *ecs)
{
    (void)ecs;
    return NU_ERROR_NONE;
}

#endif

#endif
