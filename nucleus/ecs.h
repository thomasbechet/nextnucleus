#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/ecs/api.h>
#include <nucleus/ecs/system.h>
#include <nucleus/ecs/component.h>

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

#ifdef NU_IMPLEMENTATION

#endif

#endif
