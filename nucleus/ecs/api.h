#ifndef NU_API_H
#define NU_API_H

#include <nucleus/slotmap.h>
#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/ecs.h>

typedef nu_handle_t nu_archetype_t;
typedef nu_handle_t nu_field_t;
typedef nu_handle_t nu_group_t;
typedef nu_handle_t nu_query_t;

NU_API nu_bool_t      nu_next_archetype(nu_api_t api, nu_archetype_t *it);
NU_API nu_bool_t      nu_next_system(nu_api_t api, nu_archetype_t *it);
NU_API nu_error_t     nu_create_archetype(nu_api_t                   api,
                                          const nu_archetype_info_t *info,
                                          nu_archetype_t            *archetype);
NU_API nu_archetype_t nu_find_archetype(nu_api_t api, nu_uid_t uid);
NU_API nu_field_t     nu_find_field(nu_api_t       api,
                                    nu_archetype_t archetype,
                                    nu_uid_t       uid);
NU_API nu_error_t     nu_archetype_info(nu_api_t             api,
                                        nu_archetype_t       archetype,
                                        nu_archetype_info_t *info);
NU_API nu_group_t     nu_create_group(nu_api_t       api,
                                      nu_archetype_t archetype,
                                      nu_u16_t       capacity);
NU_API nu_entity_t    nu_spawn(nu_api_t api, nu_group_t group);
NU_API void          *nu_field(nu_api_t api, nu_entity_t e, nu_field_t f);

NU_API nu_error_t nu_load_bundle(nu_api_t api, )

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu__allocator_t *allocator;
    nu__ecs_t       *ecs;
} nu__api_t;

nu_bool_t
nu_next_component (nu_api_t api, nu_component_t *it)
{
    nu__api_t *papi = api;
    if (*it)
    {
        return (*it = nu__slotlist_next(papi->ecs->field_list,
                                        nu__component_slot(*it)));
    }
    else
    {
        return (*it = nu__component_handle(papi->ecs->first_field));
    }
}
nu_bool_t
nu_next_archetype (nu_api_t api, nu_archetype_t *it)
{
    nu__api_t *papi = api;
    if (*it)
    {
        return (*it = nu__slotlist_next(papi->ecs->archetype_list,
                                        nu__archetype_slot(*it)));
    }
    else
    {
        return (*it = nu__archetype_handle(papi->ecs->first_archetype));
    }
}
nu_bool_t
nu_next_system (nu_api_t api, nu_archetype_t *it)
{
    (void)api;
    (void)it;
    return NU_FALSE;
}

nu_handle_t
nu_find_component (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__component_handle(nu__ecs_find_component(papi->ecs, uid));
}
nu_archetype_t
nu_find_archetype (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__archetype_handle(nu__ecs_find_archetype(papi->ecs, uid));
}
nu_handle_t
nu_find_system (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__system_handle(nu__ecs_find_system(papi->ecs, uid));
}

nu_error_t
nu_component_info (nu_api_t             api,
                   nu_component_t       component,
                   nu_component_info_t *info)
{
    nu__api_t             *papi  = api;
    nu__slot_t             slot  = nu__component_slot(component);
    nu__component_entry_t *entry = nu__slotmap_get(papi->ecs->fields, slot);
    info->name                   = NU_NULL;
    if (papi->ecs->info.load_names)
    {
        info->name = nu_ident_str(papi->ecs->field_names[nu_slot_index(slot)]);
    }
    info->type = entry->type;
    info->size = entry->array_size;
    return NU_ERROR_NONE;
}
const nu_char_t *
nu_archetype_name (nu_api_t api, nu_archetype_t archetype)
{
    nu__api_t *papi = api;
    nu__slot_t slot = nu__archetype_slot(archetype);
    if (papi->ecs->info.load_names)
    {
        return nu_ident_str(papi->ecs->archetype_names[nu_slot_index(slot)]);
    }
    return NU_NULL;
}

nu_group_t
nu_create_group (nu_api_t api, nu_archetype_t archetype, nu_u16_t capacity)
{
    nu__api_t *papi = api;
    return nu__ecs_create_group(
        papi->ecs, papi->allocator, archetype, capacity);
}
nu_entity_t
nu_spawn (nu_api_t api, nu_group_t group)
{
    nu__api_t *papi = api;
    return nu__ecs_spawn(papi->ecs, group);
}
void *
nu_field (nu_api_t api, nu_entity_t e, nu_component_t c)
{
    nu__api_t *papi = api;
    return nu__ecs_field(papi->ecs, e, c);
}
nu_query_t
nu_query (nu_api_t api, const nu_component_t *components, nu_u16_t count)
{
    (void)api;
    (void)components;
    (void)count;
    return NU_NULL;
}
nu_bool_t
nu_group_next (nu_api_t api, nu_entity_t *entity)
{
    (void)api;
    (void)entity;
    return NU_FALSE;
}
nu_bool_t
nu_query_next (nu_api_t api, nu_query_t query, nu_entity_t *it)
{
    (void)api;
    (void)query;
    (void)it;
    return NU_FALSE;
}

nu_error_t
nu_draw (nu_api_t api)
{
    (void)api;
    return NU_ERROR_NONE;
}

#endif

#endif
