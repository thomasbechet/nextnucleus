#ifndef NU_ECS_H
#define NU_ECS_H

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

nu_error_t     nu__ecs_init(nu__allocator_t *alloc, nu__ecs_t *ecs);
nu_error_t     nu__ecs_free(nu__ecs_t *ecs);
nu_error_t     nu__ecs_register_component(nu__ecs_t                 *ecs,
                                          nu__allocator_t           *alloc,
                                          const nu_component_info_t *info,
                                          nu_component_t            *handle);
nu_component_t nu__ecs_find_component(nu__ecs_t *ecs, const nu_char_t *name);
nu_error_t     nu__ecs_register_property(nu__ecs_t                *ecs,
                                         nu__allocator_t          *alloc,
                                         nu_component_t            component,
                                         const nu_property_info_t *info,
                                         nu_property_t            *handle);
nu_property_t  nu__ecs_find_property(nu_component_t   component,
                                     const nu_char_t *name);

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
    struct nu__component *component;

    /* find duplicated component */
    if (nu__ecs_find_component(ecs, info->name))
    {
        return NU_ERROR_DUPLICATED_ENTRY;
    }

    /* TODO: check duplicated properties name */

    /* insert component */
    component = nu__list_append(&ecs->components, alloc, NU_ALLOC_FLAG_CORE);
    nu_ident_set_str(component->name, info->name);
    component->first_property = NU_NULL;

    if (handle)
    {
        *handle = component;
    }

    return NU_ERROR_NONE;
}
nu_component_t
nu__ecs_find_component (nu__ecs_t *ecs, const nu_char_t *name)
{
    struct nu__component *it = nu__list_first(&ecs->components);
    while (it)
    {
        if (nu_strncmp(nu_ident_str(it->name), name, NU_IDENT_MAX))
        {
            return it;
        }
        it = nu__list_next(it);
    }
    return NU_NULL;
}
nu_error_t
nu__ecs_register_property (nu__ecs_t                *ecs,
                           nu__allocator_t          *alloc,
                           nu_component_t            component,
                           const nu_property_info_t *info,
                           nu_property_t            *handle)
{
    struct nu__property *prop;

    /* TODO: check duplicated entry */

    prop       = nu__list_append(&ecs->properties, alloc, NU_ALLOC_FLAG_CORE);
    prop->type = info->type;
    prop->kind = info->kind;
    prop->next = NU_NULL;

    if (component->first_property)
    {
        component->first_property->next = prop;
    }
    else
    {
        component->first_property = prop;
    }

    if (handle)
    {
        *handle = prop;
    }

    return NU_ERROR_NONE;
}
nu_property_t
nu__ecs_find_property (nu_component_t component, const nu_char_t *name)
{
    struct nu__property *it = component->first_property;
    while (it)
    {
        if (nu_strncmp(nu_ident_str(it->name), name, NU_IDENT_MAX))
        {
            return it;
        }
        it = it->next;
    }
    return NU_NULL;
}

#endif

#endif
