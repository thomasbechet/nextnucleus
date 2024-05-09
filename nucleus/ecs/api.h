#ifndef NU_API_H
#define NU_API_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/platform.h>
#include <nucleus/ecs/component.h>
#include <nucleus/ecs/system.h>

NU_API nu_error_t nu_register_system(nu_api_t                api,
                                     const nu_system_info_t *info);
NU_API nu_error_t nu_register_component(nu_api_t                   api,
                                        const nu_component_info_t *info);

NU_API nu_error_t nu_draw(nu_api_t api);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu_register_system (nu_api_t api, const nu_system_info_t *info)
{
    (void)api;
    (void)info;
    return NU_ERROR_NONE;
}
nu_error_t
nu_register_component (nu_api_t api, const nu_component_info_t *info)
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
