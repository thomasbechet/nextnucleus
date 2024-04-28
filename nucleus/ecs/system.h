#ifndef NU_SYSTEM_H
#define NU_SYSTEM_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/macro.h>
#include <nucleus/string.h>

typedef struct nu_api    *nu_api_t;
typedef struct nu_system *nu_system_t;

#define NU_SYSTEM_MAX    256
#define NU_COMPONENT_MAX 128

typedef nu_error_t (*nu_system_init_pfn_t)(void *sys);
typedef nu_error_t (*nu_system_run_pfn_t)(const void *sys, nu_api_t api);

typedef struct
{
    const nu_char_t     *ident;
    nu_size_t            size;
    nu_system_init_pfn_t init;
    nu_system_run_pfn_t  run;
} nu_system_info_t;

#ifdef NU_IMPLEMENTATION

struct nu_system
{
    nu_ident_t           ident;
    nu_size_t            size;
    nu_system_init_pfn_t init;
    nu_system_run_pfn_t  run;
};

#endif

#endif
