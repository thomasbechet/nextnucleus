#ifndef NU_API_H
#define NU_API_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/ecs.h>

NU_API nu_error_t     nu_register_component(nu_api_t                   api,
                                            const nu_component_info_t *info,
                                            nu_component_t            *handle);
NU_API nu_component_t nu_find_component(nu_api_t api, const char *name);
NU_API nu_error_t     nu_register_property(nu_api_t                  api,
                                           nu_component_t            component,
                                           const nu_property_info_t *info,
                                           nu_property_t            *handle);
NU_API nu_property_t  nu_find_property(nu_api_t       api,
                                       nu_component_t component,
                                       const char    *name);
NU_API nu_error_t     nu_register_system(nu_api_t                api,
                                         const nu_system_info_t *info);

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

struct nu_api
{
    nu__allocator_t *allocator;
    nu__ecs_t       *ecs;
};

nu_error_t
nu_register_component (nu_api_t                   api,
                       const nu_component_info_t *info,
                       nu_component_t            *handle)
{
    return nu__ecs_register_component(api->ecs, api->allocator, info, handle);
}
nu_component_t
nu_find_component (nu_api_t api, const char *name)
{
    return nu__ecs_find_component(api->ecs, name);
}
nu_error_t
nu_register_property (nu_api_t                  api,
                      nu_component_t            component,
                      const nu_property_info_t *info,
                      nu_property_t            *handle)
{
    return nu__ecs_register_property(
        api->ecs, api->allocator, component, info, handle);
}
nu_property_t
nu_find_property (nu_api_t api, nu_component_t component, const char *name)
{
    (void)api;
    return nu__ecs_find_property(component, name);
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
