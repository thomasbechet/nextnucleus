#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/allocator.h>
#include <nucleus/ecs/system.h>
#include <nucleus/ecs/component.h>
#include <nucleus/list.h>
#include <nucleus/slotmap.h>
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
    nu_u16_t max_component_count;
    nu_u16_t max_property_count;
} nu_ecs_info_t;

typedef struct
{
    nu__slotmap_t  components;
    nu__slotlist_t component_list;
    nu__slot_t     first_component;
    nu__slotmap_t  properties;
    nu__slotlist_t property_lists;
    nu_ecs_info_t  info;
} nu__ecs_t;

nu_error_t nu__ecs_init(const nu_ecs_info_t *info,
                        nu__allocator_t     *alloc,
                        nu__ecs_t           *ecs);
nu_error_t nu__ecs_free(nu__ecs_t *ecs);
nu_error_t nu__ecs_register_component(nu__ecs_t                 *ecs,
                                      const nu_component_info_t *info,
                                      nu__slot_t                *slot);
nu__slot_t nu__ecs_find_component(nu__ecs_t *ecs, const nu_char_t *name);
nu_error_t nu__ecs_register_property(nu__ecs_t                *ecs,
                                     nu__slot_t                component,
                                     const nu_property_info_t *info,
                                     nu__slot_t               *slot);
nu__slot_t nu__ecs_find_property(nu__ecs_t       *ecs,
                                 nu__slot_t       component,
                                 const nu_char_t *name);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu__ecs_init (const nu_ecs_info_t *info, nu__allocator_t *alloc, nu__ecs_t *ecs)
{
    (void)alloc;
    (void)ecs;

    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__component_entry_t),
                     info->max_component_count,
                     &ecs->components);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->max_component_count,
                      &ecs->component_list);
    ecs->first_component = NU_SLOT_NULL;
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__property_entry_t),
                     info->max_property_count,
                     &ecs->properties);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->max_property_count,
                      &ecs->property_lists);
    ecs->info = *info;

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
                            const nu_component_info_t *info,
                            nu__slot_t                *slot)
{
    nu__component_entry_t *entry;

    /* find duplicated component */
    if (nu__ecs_find_component(ecs, info->name))
    {
        return NU_ERROR_DUPLICATED_ENTRY;
    }

    /* TODO: check duplicated properties name */

    /* insert component */
    *slot = nu__slotmap_add(ecs->components);
    entry = nu__slotmap_get(ecs->components, *slot);
    nu_ident_set_str(entry->name, info->name);
    entry->first_property = NU_SLOT_NULL;

    return NU_ERROR_NONE;
}
nu__slot_t
nu__ecs_find_component (nu__ecs_t *ecs, const nu_char_t *name)
{
    nu__slot_t it = ecs->first_component;
    while (it)
    {
        nu__component_entry_t *entry = nu__slotmap_get(ecs->components, it);
        if (nu_strncmp(nu_ident_str(entry->name), name, NU_IDENT_MAX))
        {
            return it;
        }
        it = nu__slotlist_next(ecs->component_list, it);
    }
    return NU_SLOT_NULL;
}
nu_error_t
nu__ecs_register_property (nu__ecs_t                *ecs,
                           nu__slot_t                component,
                           const nu_property_info_t *info,
                           nu__slot_t               *slot)
{
    nu__property_entry_t  *entry;
    nu__component_entry_t *component_entry;

    /* TODO: check duplicated entry */

    *slot       = nu__slotmap_add(ecs->properties);
    entry       = nu__slotmap_get(ecs->properties, *slot);
    entry->type = info->type;
    entry->kind = info->kind;

    component_entry = nu__slotmap_get(ecs->components, component);
    nu__slotlist_add_first(
        ecs->property_lists, &component_entry->first_property, *slot);

    return NU_ERROR_NONE;
}
nu__slot_t
nu__ecs_find_property (nu__ecs_t       *ecs,
                       nu__slot_t       component,
                       const nu_char_t *name)
{
    nu__slot_t it
        = ((nu__component_entry_t *)nu__slotmap_get(ecs->components, component))
              ->first_property;
    while (it)
    {
        nu__property_entry_t *entry = nu__slotmap_get(ecs->properties, it);
        if (nu_strncmp(nu_ident_str(entry->name), name, NU_IDENT_MAX))
        {
            return it;
        }
        it = nu__slotlist_next(ecs->property_lists, it);
    }
    return NU_SLOT_NULL;
}

#endif

#endif
