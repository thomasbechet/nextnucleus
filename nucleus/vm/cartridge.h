#ifndef NU_CARTRIDGE_H
#define NU_CARTRIDGE_H

#include <nucleus/resource.h>
#include <nucleus/vm/property.h>
#include <nucleus/vm/types.h>
#include <nucleus/vm/error.h>

typedef nu_u32_t nu_resource_t;

typedef struct
{
    const nu_char_t   *name;
    nu_resource_type_t type;
} nu_resource_info_t;

typedef struct
{
    void *userdata;

    nu_error_t (*load_vm_properties)(void               *userdata,
                                     nu_vm_properties_t *properties);

    nu_error_t (*resource_info)(void               *userdata,
                                nu_resource_t       resource,
                                nu_resource_info_t *info);
    nu_error_t (*load_resource)(void         *userdata,
                                nu_resource_t resource,
                                void         *data);

} nu_cartridge_api_t;

#ifdef NU_IMPL

typedef struct
{
    nu_cartridge_api_t api;
} nu__cartdrige_t;

#endif

#endif
