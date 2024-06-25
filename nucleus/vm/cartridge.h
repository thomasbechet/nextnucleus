#ifndef NU_CARTRIDGE_H
#define NU_CARTRIDGE_H

#include <nucleus/vm/property.h>
#include <nucleus/vm/types.h>
#include <nucleus/vm/error.h>

typedef nu_handle_t nu_bundle_t;

typedef struct
{
    const nu_char_t *name;
    nu_u16_t         texture_count;
} nu_bundle_info_t;

typedef struct
{
    nu_u16_t width;
    nu_u16_t height;
} nu_texture_info_t;

typedef struct
{
    void *userdata;

    nu_error_t (*load_properties)(void               *userdata,
                                  nu_vm_properties_t *properties);

    nu_error_t (*next_bundle)(nu_u32_t *it, void *userdata);
    nu_error_t (*bundle_info)(nu_u32_t          id,
                              void             *userdata,
                              nu_bundle_info_t *info);

    nu_error_t (*next_texture)(nu_u32_t id, void *userdata, nu_u32_t *it);
    nu_error_t (*texture_info)(nu_u32_t           id,
                               void              *userdata,
                               nu_texture_info_t *info);
    nu_error_t (*load_texture)(nu_u32_t id, void *userdata, void *data);
} nu_cartridge_info_t;

#ifdef NU_IMPL

#endif

#endif
