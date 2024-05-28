#ifndef NU_API_H
#define NU_API_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/ecs.h>

NU_API nu_error_t  nu_register_component(nu_api_t                   api,
                                         const nu_component_info_t *info,
                                         nu_handle_t               *handle);
NU_API nu_handle_t nu_find_component(nu_api_t api, nu_uid_t uid);

NU_API nu_error_t nu_register_archetype(nu_api_t                   api,
                                        const nu_archetype_info_t *info,
                                        nu_handle_t               *handle);

NU_API nu_error_t  nu_register_system(nu_api_t                api,
                                      const nu_system_info_t *info,
                                      nu_handle_t            *handle);
NU_API nu_handle_t nu_find_system(nu_api_t api, nu_uid_t uid);

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu__allocator_t *allocator;
    nu__ecs_t       *ecs;
} nu__api_t;

nu_error_t
nu_register_component (nu_api_t                   api,
                       const nu_component_info_t *info,
                       nu_handle_t               *handle)
{
    nu__slot_t slot;
    nu_error_t error;
    nu__api_t *papi = api;
    error           = nu__ecs_register_component(papi->ecs, info, &slot);
    NU_ERROR_CHECK(error, return error);
    if (handle)
    {
        *handle = nu__component_handle(slot);
    }
    return error;
}
nu_handle_t
nu_find_component (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__component_handle(nu__ecs_find_component(papi->ecs, uid));
}

nu_error_t
nu_register_archetype (nu_api_t                   api,
                       const nu_archetype_info_t *info,
                       nu_handle_t               *handle)
{
    nu__slot_t slot;
    nu_error_t error;
    nu__api_t *papi = api;
    error           = nu__ecs_register_archetype(papi->ecs, papi->allocator, info, &slot);
    NU_ERROR_CHECK(error, return error);
    if (handle)
    {
        *handle = nu__archetype_handle(slot);
    }
    return error;
}

nu_error_t
nu_register_system (nu_api_t                api,
                    const nu_system_info_t *info,
                    nu_handle_t            *handle)
{
    nu__slot_t slot;
    nu_error_t error;
    nu__api_t *papi = api;
    error           = nu__ecs_register_system(papi->ecs, info, &slot);
    NU_ERROR_CHECK(error, return error);
    if (handle)
    {
        *handle = nu__system_handle(slot);
    }
    return error;
}
nu_handle_t
nu_find_system (nu_api_t api, nu_uid_t uid)
{
    nu__api_t *papi = api;
    return nu__system_handle(nu__ecs_find_system(papi->ecs, uid));
}

nu_error_t
nu_draw (nu_api_t api)
{
    (void)api;
    return NU_ERROR_NONE;
}

#endif

#endif
