#ifndef NU_API_H
#define NU_API_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/ecs.h>

NU_API nu_error_t     nu_register_component(nu_api_t         api,
                                            const nu_char_t *name,
                                            nu_component_t  *handle);
NU_API nu_component_t nu_find_component(nu_api_t api, const nu_char_t *name);
NU_API nu_component_t nu_find_component_uid(nu_api_t api, nu_uid_t uid);

NU_API nu_error_t    nu_register_property(nu_api_t         api,
                                          nu_component_t   component,
                                          const nu_char_t *name,
                                          nu_type_t        type,
                                          nu_property_t   *handle);
NU_API nu_property_t nu_find_property_uid(nu_api_t       api,
                                          nu_component_t component,
                                          nu_uid_t       uid);
NU_API nu_property_t nu_find_property(nu_api_t       api,
                                      nu_component_t component,
                                      const char    *name);

NU_API nu_error_t nu_register_system(nu_api_t                api,
                                     const nu_system_info_t *info);

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

struct nu_api
{
    nu__allocator_t *allocator;
    nu__ecs_t       *ecs;
};

NU_API nu_error_t
nu_register_component (nu_api_t         api,
                       const nu_char_t *name,
                       nu_component_t  *handle)
{
    nu__slot_t slot;
    nu_error_t error;
    error = nu__ecs_register_component(api->ecs, name, &slot);
    NU_ERROR_CHECK(error, return error);
    if (handle)
    {
        handle->slot = slot;
    }
    return error;
}
nu_component_t
nu_find_component (nu_api_t api, const char *name)
{
    return nu_find_component_uid(api, nu_uid(name));
}
nu_component_t
nu_find_component_uid (nu_api_t api, nu_uid_t uid)
{
    nu_component_t handle;
    handle.slot = nu__ecs_find_component(api->ecs, uid);
    return handle;
}

nu_error_t
nu_register_property (nu_api_t         api,
                      nu_component_t   component,
                      const nu_char_t *name,
                      nu_type_t        type,
                      nu_property_t   *handle)
{
    nu__slot_t slot;
    nu_error_t error;
    error = nu__ecs_register_property(
        api->ecs, component.slot, name, type, &slot);
    NU_ERROR_CHECK(error, return error);
    if (handle)
    {
        handle->slot = slot;
    }
    return error;
}
nu_property_t
nu_find_property (nu_api_t api, nu_component_t component, const nu_char_t *name)
{
    return nu_find_property_uid(api, component, nu_uid(name));
}
nu_property_t
nu_find_property_uid (nu_api_t api, nu_component_t component, nu_uid_t uid)
{
    nu_property_t handle;
    handle.slot = nu__ecs_find_property(api->ecs, component.slot, uid);
    return handle;
}

nu_error_t
nu_register_system (nu_api_t api, const nu_system_info_t *info)
{
    (void)api;
    (void)info;
    return NU_ERROR_NONE;
}

nu_error_t
nu_draw (nu_api_t api)
{
    (void)api;
    return NU_ERROR_NONE;
}

#endif

#endif
