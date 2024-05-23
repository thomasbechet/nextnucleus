#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/allocator.h>
#include <nucleus/ecs/system.h>
#include <nucleus/ecs/component.h>
#include <nucleus/list.h>
#include <nucleus/slotmap.h>
#include <nucleus/string.h>

/* typedef struct
{

} nu_query_iter_t; */

typedef struct
{
    nu_u16_t  component_capacity;
    nu_u16_t  property_capacity;
    nu_u16_t  system_capacity;
    nu_bool_t load_names;
} nu_ecs_info_t;

#define NU_DEFAULT_COMPONENT_CAPACITY 128
#define NU_DEFAULT_PROPERTY_CAPACITY  512
#define NU_DEFAULT_SYSTEM_CAPACITY    128

typedef struct
{
    nu__slotmap_t  components;
    nu__slotlist_t component_list;
    nu_ident_t    *component_names;
    nu__slot_t     first_component;

    nu__slotmap_t  properties;
    nu__slotlist_t property_lists;
    nu_ident_t    *property_names;

    nu_ecs_info_t info;
} nu__ecs_t;

NU_API void nu_ecs_info_default(nu_ecs_info_t *info);

nu_error_t nu__ecs_init(const nu_ecs_info_t *info,
                        nu__allocator_t     *alloc,
                        nu__ecs_t           *ecs);
nu_error_t nu__ecs_free(nu__ecs_t *ecs);

nu_error_t nu__ecs_register_component(nu__ecs_t       *ecs,
                                      const nu_char_t *name,
                                      nu__slot_t      *slot);
nu__slot_t nu__ecs_find_component(nu__ecs_t *ecs, nu_uid_t uid);

nu_error_t nu__ecs_register_property(nu__ecs_t       *ecs,
                                     nu__slot_t       component,
                                     const nu_char_t *name,
                                     nu_type_t        type,
                                     nu__slot_t      *slot);
nu__slot_t nu__ecs_find_property(nu__ecs_t *ecs,
                                 nu__slot_t component,
                                 nu_uid_t   uid);

#ifdef NU_IMPLEMENTATION

void
nu_ecs_info_default (nu_ecs_info_t *info)
{
    info->component_capacity = NU_DEFAULT_COMPONENT_CAPACITY;
    info->property_capacity  = NU_DEFAULT_PROPERTY_CAPACITY;
    info->system_capacity    = NU_DEFAULT_SYSTEM_CAPACITY;
    info->load_names         = NU_TRUE;
}

nu_error_t
nu__ecs_init (const nu_ecs_info_t *info, nu__allocator_t *alloc, nu__ecs_t *ecs)
{
    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__component_entry_t),
                     info->component_capacity,
                     &ecs->components);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->component_capacity,
                      &ecs->component_list);
    if (info->load_names)
    {
        ecs->component_names
            = nu__alloc(alloc,
                        sizeof(nu_ident_t) * info->component_capacity,
                        NU_MEMORY_USAGE_ECS);
    }
    ecs->first_component = NU_SLOT_NULL;

    nu__slotmap_init(alloc,
                     NU_MEMORY_USAGE_ECS,
                     sizeof(nu__property_entry_t),
                     info->property_capacity,
                     &ecs->properties);
    nu__slotlist_init(alloc,
                      NU_MEMORY_USAGE_ECS,
                      info->property_capacity,
                      &ecs->property_lists);
    if (info->load_names)
    {
        ecs->property_names
            = nu__alloc(alloc,
                        sizeof(nu_ident_t) * info->property_capacity,
                        NU_MEMORY_USAGE_ECS);
    }

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
nu__ecs_register_component (nu__ecs_t       *ecs,
                            const nu_char_t *name,
                            nu__slot_t      *slot)
{
    nu__component_entry_t *entry;
    nu_uid_t               uid = nu_uid(name);

    /* find duplicated component */
    if (nu__ecs_find_component(ecs, uid))
    {
        return NU_ERROR_DUPLICATED_ENTRY;
    }

    /* insert component */
    *slot                 = nu__slotmap_add(ecs->components);
    entry                 = nu__slotmap_get(ecs->components, *slot);
    entry->uid            = uid;
    entry->first_property = NU_SLOT_NULL;

    if (ecs->info.load_names)
    {
        nu_ident_set_str(ecs->component_names[nu_slot_index(*slot)], name);
    }

    return NU_ERROR_NONE;
}
nu__slot_t
nu__ecs_find_component (nu__ecs_t *ecs, nu_uid_t uid)
{
    nu__slot_t it = ecs->first_component;
    while (it)
    {
        nu__component_entry_t *entry = nu__slotmap_get(ecs->components, it);
        if (entry->uid == uid)
        {
            return it;
        }
        it = nu__slotlist_next(ecs->component_list, it);
    }
    return NU_SLOT_NULL;
}
nu_error_t
nu__ecs_register_property (nu__ecs_t       *ecs,
                           nu__slot_t       component,
                           const nu_char_t *name,
                           nu_type_t        type,
                           nu__slot_t      *slot)
{
    nu__property_entry_t  *entry;
    nu__component_entry_t *component_entry;
    nu_uid_t               uid = nu_uid(name);

    /* TODO: check duplicated entry */

    *slot       = nu__slotmap_add(ecs->properties);
    entry       = nu__slotmap_get(ecs->properties, *slot);
    entry->uid  = uid;
    entry->type = type;
    entry->kind = NU_PROPERTY_SCALAR;

    if (ecs->info.load_names)
    {
        nu_ident_set_str(ecs->property_names[nu_slot_index(*slot)], name);
    }

    component_entry = nu__slotmap_get(ecs->components, component);
    nu__slotlist_add_first(
        ecs->property_lists, &component_entry->first_property, *slot);

    return NU_ERROR_NONE;
}
nu__slot_t
nu__ecs_find_property (nu__ecs_t *ecs, nu__slot_t component, nu_uid_t uid)
{
    nu__slot_t it
        = ((nu__component_entry_t *)nu__slotmap_get(ecs->components, component))
              ->first_property;
    while (it)
    {
        nu__property_entry_t *entry = nu__slotmap_get(ecs->properties, it);
        if (entry->uid == uid)
        {
            return it;
        }
        it = nu__slotlist_next(ecs->property_lists, it);
    }
    return NU_SLOT_NULL;
}

#endif

#endif
