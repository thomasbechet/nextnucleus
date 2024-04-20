#ifndef NU_COMPONENT_H
#define NU_COMPONENT_H

#include <nucleus/types.h>
#include <nucleus/macro.h>
#include <nucleus/string.h>

NU_DECLARE_HANDLE(nu_component_t);

typedef struct
{
    const nu_u8_t *ident;
    nu_size_t size;
} nu_component_info_t;

typedef struct
{
    nu_ident_t ident;
    nu_size_t size;
} nu__component_t;

#endif
