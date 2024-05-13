#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/ecs/api.h>
#include <nucleus/ecs/system.h>
#include <nucleus/ecs/component.h>
#include <nucleus/list.h>
#include <nucleus/string.h>

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
nu_error_t nu__ecs_register_component(nu__ecs_t                 *ecs,
                                      nu__allocator_t           *alloc,
                                      const nu_component_info_t *info,
                                      nu_component_t            *handle);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu__ecs_init (nu__allocator_t *alloc, nu__ecs_t *ecs)
{
    (void)alloc;
    (void)ecs;

    nu__list_init(&ecs->archetypes, sizeof(struct nu__property));
    nu__list_init(&ecs->properties, sizeof(struct nu__property));
    nu__list_init(&ecs->components, sizeof(struct nu__component));

    return NU_ERROR_NONE;
}

nu_error_t
nu__ecs_free (nu__ecs_t *ecs)
{
    (void)ecs;
    return NU_ERROR_NONE;
}

nu_error_t
nu__ecs_register_component (nu__ecs_t                 *ecs,
                            nu__allocator_t           *alloc,
                            const nu_component_info_t *info,
                            nu_component_t            *handle)
{
    struct nu__component *it;
    struct nu__component *component;
    struct nu__property  *prev;
    nu_size_t             i;

    /* find duplicated component */
    it = nu__list_first(&ecs->components);
    while (it)
    {
        if (nu_strncmp(nu_ident_str(it->ident), info->ident, NU_IDENT_MAX))
        {
            return NU_ERROR_DUPLICATED_ENTRY;
        }
        it = nu__list_next(&ecs->components, it);
    }

    /* TODO: check duplicated properties name */

    /* insert component */
    component = nu__list_append(&ecs->components, alloc, NU_ALLOC_FLAG_CORE);
    nu_ident_set_str(component->ident, info->ident);
    component->first_property = NU_NULL;

    /* insert properties */
    for (i = 0; i < info->property_count; ++i)
    {
        struct nu__property *prop
            = nu__list_append(&ecs->properties, alloc, NU_ALLOC_FLAG_CORE);
        nu_ident_set_str(prop->ident, info->properties[i].ident);
        prop->type = info->properties[i].type;
        prop->kind = info->properties[i].kind;
        if (i == 0)
        {
            component->first_property = prop;
        }
        else
        {
            prev->next = prop;
        }
        prev = prop;
    }

    *handle = component;

    return NU_ERROR_NONE;
}

#endif

#endif
