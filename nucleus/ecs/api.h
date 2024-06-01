#ifndef NU_API_H
#define NU_API_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/ecs.h>

typedef nu_handle_t nu_component_t;
typedef nu_handle_t nu_archetype_t;
typedef nu_handle_t nu_group_t;
typedef nu_handle_t nu_query_t;

NU_API nu_component_t nu_find_component(nu_api_t api, nu_uid_t uid);
NU_API nu_component_t nu_find_system(nu_api_t api, nu_uid_t uid);

NU_API nu_group_t  nu_create_group(nu_api_t       api,
                                   nu_archetype_t archetype,
                                   nu_u16_t       capacity);
NU_API nu_entity_t nu_spawn(nu_api_t api, nu_group_t group);
NU_API void       *nu_field(nu_api_t api, nu_entity_t e, nu_component_t c);
NU_API nu_query_t  nu_query(nu_api_t             api,
                            const nu_component_t components,
                            nu_u16_t             count);
NU_API nu_bool_t   nu_group_next(nu_api_t api, nu_entity_t *entity);
NU_API nu_bool_t   nu_query_next(nu_api_t api, nu_entity_t *it);

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu__allocator_t *allocator;
    nu__ecs_t       *ecs;
} nu__api_t;

nu_handle_t
nu_find_component (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__component_handle(nu__ecs_find_component(papi->ecs, uid));
}
nu_handle_t
nu_find_system (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__system_handle(nu__ecs_find_system(papi->ecs, uid));
}

nu_group_t
nu_create_group (nu_api_t api, nu_archetype_t archetype, nu_u16_t capacity)
{
    nu__api_t  *papi = api;
    nu_handle_t handle
        = nu__ecs_create_group(papi->ecs, papi->allocator, archetype, capacity);
    return NU_NULL;
}
nu_entity_t
nu_spawn (nu_api_t api, nu_group_t group)
{
    return NU_NULL;
}
void *
nu_field (nu_api_t api, nu_entity_t e, nu_component_t c)
{
    return NU_NULL;
}
nu_query_t
nu_query (nu_api_t api, const nu_component_t components, nu_u16_t count)
{
    return NU_NULL;
}
nu_bool_t
nu_group_next (nu_api_t api, nu_entity_t *entity)
{
    return NU_FALSE;
}
nu_bool_t
nu_query_next (nu_api_t api, nu_entity_t *it)
{
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
