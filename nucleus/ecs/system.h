#ifndef NU_SYSTEM_H
#define NU_SYSTEM_H

#include <nucleus/types.h>
#include <nucleus/error.h>
#include <nucleus/string.h>
#include <nucleus/ecs/component.h>

#define NU_SYSTEM_COMPONENT_CAPACITY 32

typedef void *nu_api_t;
typedef nu_error_t (*nu_system_callback_pfn_t)(nu_api_t api);

typedef enum
{
    NU_COMPONENT_READ,
    NU_COMPONENT_WRITE
} nu_component_access_t;

typedef struct
{
    nu_u16_t              slot;
    nu_handle_t           handle;
    nu_component_access_t access;
} nu_system_component_t;

typedef struct
{
    const char                  *name;
    nu_system_callback_pfn_t     callback;
    const nu_system_component_t *components;
    nu_size_t                    component_count;
} nu_system_info_t;

#ifdef NU_IMPLEMENTATION

typedef struct
{
    nu__slot_t            slot;
    nu_component_access_t access;
} nu__system_component_t;

typedef struct
{
    nu__slot_t slot;
    nu__slot_t archetype;
} nu__system_entites_t;

typedef struct
{
    nu_uid_t                 uid;
    nu_system_callback_pfn_t callback;
    nu__system_component_t   components[NU_SYSTEM_COMPONENT_CAPACITY];
    nu_u16_t                 component_count;
} nu__system_entry_t;

static nu_handle_t
nu__system_handle (nu__slot_t slot)
{
    return slot;
}

#endif

#endif
