#ifndef NU_SYSTEM_H
#define NU_SYSTEM_H

#include <nucleus/types.h>
#include <nucleus/macro.h>
#include <nucleus/string.h>

NU_DECLARE_HANDLE(nu_system_t);
NU_DECLARE_HANDLE(nu_api_t);

typedef nu_result_t (*nu_system_init_pfn_t)(void *sys);
typedef nu_result_t (*nu_system_run_pfn_t)(const void *sys, nu_api_t api);

typedef struct
{
    const nu_u8_t *ident;
    nu_size_t size;
    nu_system_init_pfn_t init;
    nu_system_run_pfn_t run;
} nu_system_info_t;

typedef struct
{
    nu_ident_t ident;
    nu_size_t size;
    nu_system_init_pfn_t init;
    nu_system_run_pfn_t run;
} nu__system_t;

#endif
