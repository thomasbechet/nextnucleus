#ifndef NU_SYSTEM_H
#define NU_SYSTEM_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/macro.h>
#include <nucleus/string.h>
#include <nucleus/ecs/component.h>

typedef struct nu_api    *nu_api_t;
typedef struct nu_system *nu_system_t;

typedef nu_error_t (*nu_system_run_pfn_t)(nu_api_t api);

typedef struct
{
    const nu_char_t   *name;
    nu_type_t          type;
    nu_property_kind_t kind;
} nu_system_property_t;

typedef struct
{
    nu_system_property_t *components;
    nu_size_t             component_count;
    nu_size_t             query_count;
} nu_system_info_t;

#ifdef NU_IMPLEMENTATION

struct nu__system_property
{
    struct nu__system_property *next;
};

struct nu__system_query
{
    struct nu__system_query *next;
};

struct nu_system
{
    nu_ident_t                  name;
    nu_size_t                   size;
    nu_system_run_pfn_t         run;
    struct nu__system_property *first_prop;
};

#endif

#endif
